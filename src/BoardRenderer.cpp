#include "BoardRenderer.hpp"
#include "Utils.hpp"

BoardRenderer::BoardRenderer()
{
	mArrowBox = Model{ U"obj/ArrowBox.obj" };
	Model::RegisterDiffuseTextures(mArrowBox, TextureDesc::MippedSRGB);
	mArrowBoxRotate = Model{ U"obj/ArrowBoxRotate.obj" };
	Model::RegisterDiffuseTextures(mArrowBoxRotate, TextureDesc::MippedSRGB);
	mArrowBoxSlide = Model{ U"obj/ArrowBoxSlide.obj" };
	Model::RegisterDiffuseTextures(mArrowBoxSlide, TextureDesc::MippedSRGB);
}

void BoardRenderer::Update(double dt)
{
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

	const Array<BlockGroup> blockGroup = board.GetGroups();

	// 通常タイルの描画
	for (int y = 0; y < size.y; ++y)
	{
		for (int x = 0; x < size.x; ++x)
		{
			const Tile& tile = board.GetTile({ x, y });
			int32 groupID = tile.GetGroupID();

			// Emptyあるいはアニメーション中のタイルはスキップ
			if (tile.GetType() == TileType::Empty) continue;
			if (isAnimating(groupID)) continue;

			ColorF tileColor = Linear::Palette::White;

			if (groupID != -1)
			{
				BlockGroup group = blockGroup[groupID];
				if (group.GetType() == GroupType::Rotate) tileColor = Linear::Palette::Steelblue;
				else if (group.GetType() == GroupType::Slide) tileColor = Linear::Palette::Khaki;
			}

			DrawSingleTile(tile, board.ToWorldPosition({ x, y }), 0.0, tileColor);
		}
	}

	// アニメーション中のタイルの描画
	for (const auto& anim : mAnims)
	{
		double progress = Min(anim.timer / anim.duration, 1.0);
		double e = EaseOutExpo(progress);

		for (const auto& group : board.GetGroups())
		{
			if (group.GetID() != anim.groupID) continue;

			if (group.GetType() == GroupType::Rotate)
			{
				double angle = Math::Lerp(anim.startAngle, anim.endAngle, e);

				for (const auto& pos : group.GetTiles())
				{
					Vec3 center = board.ToWorldPosition(pos);
					Vec3 currentCenter = Utils::CalcOrbitPosition(anim.pivotWorldPos, center, angle);
					
					const Tile& tile = board.GetTile(pos);
					int32 groupID = tile.GetGroupID();

					ColorF tileColor = Linear::Palette::White;

					if (groupID != -1)
					{
						if (group.GetType() == GroupType::Rotate) tileColor = Linear::Palette::Steelblue;
					}

					DrawSingleTile(tile, currentCenter, angle, tileColor);
				}
				break;
			}
			else if (group.GetType() == GroupType::Slide)
			{
				for (const auto& pos : group.GetTiles())
				{
					Vec3 targetCenter = board.ToWorldPosition(pos);
					Vec3 startCenter = board.ToWorldPosition(pos - GetOffset(group.GetSlideDirection()));
					Vec3 currentCenter = Math::Lerp(startCenter, targetCenter, e);
					
					const Tile& tile = board.GetTile(pos);
					int32 groupID = tile.GetGroupID();

					ColorF tileColor = Linear::Palette::White;

					if (groupID != -1)
					{
						if (group.GetType() == GroupType::Slide) tileColor = Linear::Palette::Khaki;
					}

					DrawSingleTile(tile, currentCenter, 0.0, tileColor);
				}
				break;
			}
		}
	}
}

void BoardRenderer::AddRotationAnim(int32 groupID, const Vec3& pivotWorldPos, double startAngle, double endAngle)
{
	mAnims.push_back({ groupID, 0.0, 0.25, pivotWorldPos, startAngle, endAngle });
}

void BoardRenderer::AddSlideAnim(int32 groupID)
{
	mAnims.push_back({ groupID, 0.0, 0.25 });
}

void BoardRenderer::DrawSingleTile(const Tile& tile, const Vec3& pos, double angleOffset, const ColorF& color) const
{
	TileType type = tile.GetType();

	// ベースの向きにアニメーション分の追加角度を足す（通常時は0）
	double baseAngle = GetBaseAngle(tile.GetDirection());
	double finalAngle = baseAngle + angleOffset;

	if (type == TileType::Normal)
	{
		OrientedBox{ pos, { 1.47, 0.3, 1.47 }, Quaternion::RotateY(finalAngle) }.draw(color);
	}
	else if (type == TileType::Goal)
	{
		OrientedBox{ pos, { 1.47, 0.3, 1.47 }, Quaternion::RotateY(finalAngle) }.draw(Linear::Palette::Blueviolet);
	}
	else if (type == TileType::Item)
	{
		OrientedBox{ pos, { 1.47, 0.3, 1.47 }, Quaternion::RotateY(finalAngle) }.draw(color);
		if (!tile.GetIsCollected())
		{
			Cylinder{ pos + Vec3{ 0, 0.5, 0 }, 0.4, 1.0 }.draw();
		}
	}
	else if (type == TileType::Arrow)
	{
		if (color == Linear::Palette::White)
		{
			mArrowBox.draw(pos, Quaternion::RotateY(finalAngle));
		}
		else if (color == Linear::Palette::Steelblue)
		{
			mArrowBoxRotate.draw(pos, Quaternion::RotateY(finalAngle));
		}
		else
		{
			mArrowBoxSlide.draw(pos, Quaternion::RotateY(finalAngle));
		}
	}
	else if (type == TileType::RotateTrigger)
	{
		OrientedBox{ pos, { 1.47, 0.3, 1.47 }, Quaternion::RotateY(finalAngle) }.draw(Linear::Palette::Orangered);
	}
	else if (type == TileType::Teleport)
	{
		OrientedBox{ pos, { 1.47, 0.3, 1.47 }, Quaternion::RotateY(finalAngle) }.draw(Linear::Palette::Hotpink);
	}
}

