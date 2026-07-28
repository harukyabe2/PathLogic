#include "PlayerRenderer.hpp"
#include "Utils.hpp"

PlayerRenderer::PlayerRenderer()
{
	mCharacter = Model{ U"obj/Player.obj" };
}

void PlayerRenderer::Update(double dt)
{
	if (IsAnimating())
	{
		mAnim.timer += dt;
		if (mAnim.timer >= mAnim.duration) mAnim.type = PlayerAnimType::None;
	}
}

void PlayerRenderer::Draw3D(const Player& player, const Board& board) const
{
	PlayerState state = player.GetState();

	if (state == PlayerState::Dead) return;

	Vec3 drawPos = Vec3::Zero();
	double spinAngle = 0.0;
	double rollingAngle = 0.0;

	double scale = 1.0;
	Vec3 offset = Vec3::Zero();

	// 状態によって描画する座標を分ける
	if (state == PlayerState::Placed || state == PlayerState::Walking)
	{
		scale = 1.0;
		offset = Vec3{ 0, 0.65, 0 };

		drawPos = board.ToWorldPosition(player.GetBoardPos());

		if (IsAnimating())
		{
			double progress = Min(mAnim.timer / mAnim.duration, 1.0);
			double e = EaseOutExpo(progress);

			if (mAnim.type == PlayerAnimType::Orbit)
			{
				double angle = Math::Lerp(mAnim.startAngle, mAnim.endAngle, e);
				drawPos = Utils::CalcOrbitPosition(mAnim.pivotWorldPos, mAnim.targetWorldPos, angle);

				spinAngle = angle;
			}
			else if (mAnim.type == PlayerAnimType::Slide)
			{
				drawPos = Math::Lerp(mAnim.startWorldPos, mAnim.targetWorldPos, e);
			}
			else if (mAnim.type == PlayerAnimType::Walk)
			{
				e = EaseOutCubic(progress);
				drawPos = Math::Lerp(mAnim.startWorldPos, mAnim.targetWorldPos, e);

				if (mAnim.type == PlayerAnimType::Walk)
				{
					rollingAngle = (1.0 - progress) * Math::TwoPi;
				}
			}
			else if (mAnim.type == PlayerAnimType::Fall)
			{
				double fallEase = EaseInQuad(progress);
				drawPos = Math::Lerp(mAnim.startWorldPos, mAnim.targetWorldPos, fallEase);
			}
			else if (mAnim.type == PlayerAnimType::TeleportOut)
			{
				double teleEase = EaseInQuad(progress);
				drawPos = Math::Lerp(mAnim.startWorldPos, mAnim.targetWorldPos, teleEase);

				spinAngle = progress * Math::TwoPi * 3.0;
			}
			else if (mAnim.type == PlayerAnimType::TeleportIn)
			{
				double teleEase = EaseOutQuad(progress);
				drawPos = Math::Lerp(mAnim.startWorldPos, mAnim.targetWorldPos, teleEase);

				spinAngle = (1.0 - progress) * Math::TwoPi * 3.0;
			}
		}
	}
	// UI待機中やドラッグ中はプレイヤーが保持している座標を使う
	else if (state == PlayerState::InUI || state == PlayerState::Dragging)
	{
		if (state == PlayerState::InUI) scale = 0.12;

		drawPos = player.GetDragWorldPos();
	}

	double baseAngle = GetBaseAngle(player.GetDirection());

	const Quaternion orientation = Quaternion::RotateX(rollingAngle) *
			Quaternion::RotateY(baseAngle + spinAngle);

	const Mat4x4 mat = Mat4x4::Scale(scale) 
					 * Mat4x4::Rotate(orientation) 
					 * Mat4x4::Translate(drawPos + offset);

	mCharacter.draw(mat);
}

void PlayerRenderer::StartRotationAnim(const Vec3& pivotWorldPos, const Vec3& targetWorldPos, double startAngle, double endAngle, double duration)
{
	mAnim.type = PlayerAnimType::Orbit;
	mAnim.timer = 0.0;
	mAnim.duration = duration;
	mAnim.pivotWorldPos = pivotWorldPos;
	mAnim.targetWorldPos = targetWorldPos;
	mAnim.startAngle = startAngle;
	mAnim.endAngle = endAngle;
}

void PlayerRenderer::StartWalkAnim(const Vec3& startWorldPos, const Vec3& targetWorldPos, double duration)
{
	mAnim.type = PlayerAnimType::Walk;
	mAnim.timer = 0.0;
	mAnim.duration = duration;
	mAnim.startWorldPos = startWorldPos;
	mAnim.targetWorldPos = targetWorldPos;
}

void PlayerRenderer::StartSlideAnim(const Vec3& startWorldPos, const Vec3& targetWorldPos, double duration)
{
	mAnim.type = PlayerAnimType::Slide;
	mAnim.timer = 0.0;
	mAnim.duration = duration;
	mAnim.startWorldPos = startWorldPos;
	mAnim.targetWorldPos = targetWorldPos;
}

void PlayerRenderer::StartFallAnim(const Vec3& startWorldPos, double dropHeight, double duration)
{
	mAnim.type = PlayerAnimType::Fall;
	mAnim.timer = 0.0;
	mAnim.duration = duration;
	mAnim.startWorldPos = startWorldPos;
	mAnim.targetWorldPos = startWorldPos - Vec3{ 0, dropHeight, 0 };
}

void PlayerRenderer::StartTeleportOutAnim(const Vec3& startWorldPos, const Vec3& targetWorldPos, double duration)
{
	mAnim.type = PlayerAnimType::TeleportOut;
	mAnim.timer = 0.0;
	mAnim.duration = duration;
	mAnim.startWorldPos = startWorldPos;
	mAnim.targetWorldPos = targetWorldPos;
}

void PlayerRenderer::StartTeleportInAnim(const Vec3& startWorldPos, const Vec3& targetWorldPos, double duration)
{
	mAnim.type = PlayerAnimType::TeleportIn;
	mAnim.timer = 0.0;
	mAnim.duration = duration;
	mAnim.startWorldPos = startWorldPos;
	mAnim.targetWorldPos = targetWorldPos;
}

void PlayerRenderer::StopAnim()
{
	mAnim.type = PlayerAnimType::None;
	mAnim.timer = 0.0;
}

bool PlayerRenderer::IsAnimating() const
{
	return (mAnim.type != PlayerAnimType::None);
}
