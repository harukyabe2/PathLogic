#include "PlayerRenderer.hpp"
#include "Utils.hpp"

void PlayerRenderer::Update()
{
	if (mAnim.isAnimating)
	{
		mAnim.timer += Scene::DeltaTime();
		if (mAnim.timer >= mAnim.duration) mAnim.isAnimating = false;
	}
}

void PlayerRenderer::DrawUI(const Player& player) const
{
	PlayerState state = player.GetState();

	if (state == PlayerState::InUI || state == PlayerState::Dragging)
	{
		Circle{ player.GetScreenPos(), 30}.draw(Linear::Palette::Orange);
	}
}

void PlayerRenderer::Draw3D(const Player& player, const Board& board) const
{
	PlayerState state = player.GetState();

	// 配置されたら3Dで表示
	if (state == PlayerState::Placed || state == PlayerState::Walking)
	{
		Vec3 drawPos = board.ToWorldPosition(player.GetBoardPos());

		if (mAnim.isAnimating)
		{
			double progress = Min(mAnim.timer / mAnim.duration, 1.0);
			double e = EaseOutExpo(progress);
			double angle = Math::Lerp(mAnim.startAngle, mAnim.endAngle, e);

			drawPos = Utils::CalcOrbitPosition(mAnim.pivotWorldPos, mAnim.targetWorldPos, angle);
		}

		Cylinder{ drawPos + Vec3{ 0, 0.5, 0 }, 0.4, 1.0 }.draw();
	}
}

void PlayerRenderer::StartRotationAnim(const Vec3& pivotWorldPos, const Vec3& targetWorldPos, double startAngle, double endAngle)
{
	mAnim.isAnimating = true;
	mAnim.timer = 0.0;
	mAnim.pivotWorldPos = pivotWorldPos;
	mAnim.targetWorldPos = targetWorldPos;
	mAnim.startAngle = startAngle;
	mAnim.endAngle = endAngle;
}
