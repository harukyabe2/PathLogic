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

			// Emptyあるいはアニメーション中のタイルはスキップ
			if (tile.GetType() == TileType::Empty) continue;
			if (isAnimating(tile.GetGroupID())) continue;

			DrawSingleTile(tile, board.ToWorldPosition({ x, y }), 0.0);
		}
	}

	// アニメーション中のタイルの描画
	for (const auto& anim : mAnims)
	{
		double progress = Min(anim.timer / anim.duration, 1.0);
		double e = EaseOutExpo(progress);
		double angle = Math::Lerp(anim.startAngle, anim.endAngle, e);

		for (const auto& group : board.GetGroups())
		{
			if (group.GetID() != anim.groupID) continue;
			
			for (const auto& pos : group.GetTiles())
			{
				Vec3 center = board.ToWorldPosition(pos);
				Vec3 currentCenter = Utils::CalcOrbitPosition(anim.pivotWorldPos, center, angle);
				
				const Tile& tile = board.GetTile(pos);

				DrawSingleTile(tile, currentCenter, angle);
			}
			break;
		}
	}
}

void BoardRenderer::AddRotationAnim(int32 groupID, const Vec3& pivotWorldPos, double startAngle, double endAngle)
{
	mAnims.push_back({ groupID, 0.0, 0.25, pivotWorldPos, startAngle, endAngle });
}

void BoardRenderer::DrawSingleTile(const Tile& tile, const Vec3& pos, double angleOffset) const
{
	TileType type = tile.GetType();

	// ベースの向きにアニメーション分の追加角度を足す（通常時は0）
	double baseAngle = GetBaseAngle(tile.GetDirection());
	double finalAngle = baseAngle + angleOffset;

	if (type == TileType::Normal)
	{
		OrientedBox{ pos, { 1.47, 0.3, 1.47 }, Quaternion::RotateY(finalAngle) }.draw();
	}
	else if (type == TileType::Goal)
	{
		OrientedBox{ pos, { 1.47, 0.3, 1.47 }, Quaternion::RotateY(finalAngle) }.draw(Linear::Palette::Blueviolet);
	}
	else if (type == TileType::Arrow)
	{
		mArrowBox.draw(pos, Quaternion::RotateY(finalAngle));
	}
}
