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

	if (state == PlayerState::Dead) return;

	// 配置されたら3Dで表示
	if (state == PlayerState::Placed || state == PlayerState::Walking)
	{
		Vec3 drawPos = board.ToWorldPosition(player.GetBoardPos());
		double spinAngle = 0.0;

		if (IsAnimating())
		{
			double progress = Min(mAnim.timer / mAnim.duration, 1.0);
			double e = EaseOutExpo(progress);

			if (mAnim.type == PlayerAnimType::Orbit)
			{
				double angle = Math::Lerp(mAnim.startAngle, mAnim.endAngle, e);
				drawPos = Utils::CalcOrbitPosition(mAnim.pivotWorldPos, mAnim.targetWorldPos, angle);
			}
			else if (mAnim.type == PlayerAnimType::Walk)
			{
				drawPos = Math::Lerp(mAnim.startWorldPos, mAnim.targetWorldPos, e);
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

		double baseAngle = GetBaseAngle(player.GetDirection());

		mCharacter.draw(drawPos, Quaternion::RotateY(baseAngle + spinAngle));

		//Cylinder{ drawPos + Vec3{ 0, 0.5, 0 }, 0.4, 1.0 }.draw();
	}
}

void PlayerRenderer::StartRotationAnim(const Vec3& pivotWorldPos, const Vec3& targetWorldPos, double startAngle, double endAngle)
{
	mAnim.type = PlayerAnimType::Orbit;
	mAnim.timer = 0.0;
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
