#include "Player.hpp"
#include "Utils.hpp"

Player::Player()
	: mState(PlayerState::InUI)
	, mUIPos(80, 80)
	, mScreenPos(mUIPos)
	, mBoardPos(-1, -1)
	, mDirection(Direction::Up)
{

}

void Player::Update()
{
	if (mAnim.isAnimating)
	{
		mAnim.timer += Scene::DeltaTime();
		if (mAnim.timer >= mAnim.duration) mAnim.isAnimating = false;
	}

	// 初期位置と掴んでいる間は2Dで表示
	if (mState == PlayerState::InUI || mState == PlayerState::Dragging)
	{
		Circle uiCircle{ mScreenPos, 30 };

		if (mState == PlayerState::InUI)
		{
			if (uiCircle.leftClicked())
			{
				mState = PlayerState::Dragging;
			}
		}
		else if (mState == PlayerState::Dragging)
		{
			mScreenPos = Cursor::Pos();
		}
	}
}

void Player::DrawUI() const
{
	if (mState == PlayerState::InUI || mState == PlayerState::Dragging)
	{
		Circle{ mScreenPos, 30 }.draw(Linear::Palette::Orange);
	}
}

void Player::Draw3D(const Vec3& worldPos) const
{
	// 配置されたら3Dで表示
	if (mState == PlayerState::Placed || mState == PlayerState::Walking)
	{
		Vec3 drawPos = worldPos;

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

void Player::ResetUIPos()
{
	mScreenPos = mUIPos;
}

void Player::StartRotationAnim(const Vec3& pivotWorldPos, const Vec3& targetWorldPos, double startAngle, double endAngle)
{
	mAnim.isAnimating = true;
	mAnim.timer = 0.0;
	mAnim.pivotWorldPos = pivotWorldPos;
	mAnim.targetWorldPos = targetWorldPos;
	mAnim.startAngle = startAngle;
	mAnim.endAngle = endAngle;
}
