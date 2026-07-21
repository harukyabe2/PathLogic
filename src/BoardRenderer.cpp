#include "BoardRenderer.hpp"
#include "Utils.hpp"

BoardRenderer::BoardRenderer()
{
	mArrowBox = Model{ U"ArrowBox.obj" };
	Model::RegisterDiffuseTextures(mArrowBox, TextureDesc::MippedSRGB);
}

void BoardRenderer::Update()
{
	double dt = Scene::DeltaTime();

	for (auto& anim : mAnims)
	{
		anim.timer += dt;
	}

	mAnims.remove_if([](const GroupAnimState& anims) { return anims.timer >= anims.duration; });
}

void BoardRenderer::Draw(const Board& board) const
{
	auto isAnimating = [&](int32 id) {
		return mAnims.contains_if([&](const GroupAnimState& anims) {
			return anims.groupID == id; });
	};

	Size size = board.GetSize();

	// 通常タイルの描画
	for (int y = 0; y < size.y; ++y)
	{
		for (int x = 0; x < size.x; ++x)
		{
			const Tile& tile = board.GetTile({ x, y });
			// Emptyのタイルあるいは
			// アニメーション中のタイルはスキップ
			TileType type = tile.GetType();
			if (type == TileType::Empty) continue;
			if (isAnimating(tile.GetGroupID())) continue;

			if (type == TileType::Normal)
			{
				Box(board.ToWorldPosition({ x, y }), {1.47, 0.3, 1.47}).draw();
			}
			else if (type == TileType::Arrow)
			{
				double baseAngle = GetBaseAngle(tile.GetDirection());
				
				mArrowBox.draw(board.ToWorldPosition({ x, y }), Quaternion::RotateY(baseAngle));
			}
		}
	}

	// アニメーション中のタイルの描画
	for (const auto& anim : mAnims)
	{
		double progress = Min(anim.timer / anim.duration, 1.0);
		double e = EaseOutExpo(progress);
		double angle = Math::Lerp(anim.startAngle, anim.endAngle, e);

		double c = std::cos(angle);
		double s = std::sin(angle);

		for (const auto& group : board.GetGroups())
		{
			if (group.GetID() != anim.groupID) continue;
			
			for (const auto& pos : group.GetTiles())
			{
				Vec3 finalCenter = board.ToWorldPosition(pos);

				Vec3 currentCenter = Utils::CalcOrbitPosition(anim.pivotWorldPos, finalCenter, angle);
				
				const Tile& tile = board.GetTile(pos);
				TileType type = tile.GetType();

				double baseAngle = GetBaseAngle(tile.GetDirection());
				// 移動した先でタイルがどちらを向いているかを計算
				double tileRotation = baseAngle + angle;

				if (type == TileType::Normal)
				{
					OrientedBox{ currentCenter, {1.47, 0.3, 1.47}, Quaternion::RotateY(tileRotation) }.draw();
				}
				else if (type == TileType::Arrow)
				{
					
					mArrowBox.draw(currentCenter, Quaternion::RotateY(tileRotation));
				}
			}
			break;
		}
	}
}

void BoardRenderer::AddRotationAnim(int32 groupID, const Vec3& pivotWorldPos, double startAngle, double endAngle)
{
	mAnims.push_back({ groupID, 0.0, 0.25, pivotWorldPos, startAngle, endAngle });
}

