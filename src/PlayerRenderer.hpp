#pragma once
#include <Siv3D.hpp>
#include "Player.hpp"
#include "Board.hpp"

// アニメーションの種類を区別
enum class PlayerAnimType
{
	None,
	Orbit,
	Walk,
	Slide,
	Fall,
	TeleportOut,
	TeleportIn
};

// アニメーションの状態を管理
struct PlayerAnimState
{
	PlayerAnimType type = PlayerAnimType::None;

	double timer = 0.0;
	double duration = 0.25;

	Vec3 startWorldPos;
	Vec3 pivotWorldPos;
	Vec3 targetWorldPos;

	double startAngle = 0.0;
	double endAngle = 0.0;
};

class PlayerRenderer
{
	public:
		PlayerRenderer();

		void Update(double dt);
		void Draw3D(const Player& player, const Board& board) const;

		void StartRotationAnim(const Vec3& pivotWorldPos, const Vec3& targetWorldPos, double startAngle, double endAngle, double duration);
		void StartWalkAnim(const Vec3& startWorldPos, const Vec3& targetWorldPos, double duration = 0.25);
		void StartSlideAnim(const Vec3& startWorldPos, const Vec3& targetWorldPos, double duration = 0.25);
		void StartFallAnim(const Vec3& startWorldPos, double dropHeight, double duration);
		void StartTeleportOutAnim(const Vec3& startWorldPos, const Vec3& targetWorldPos, double duration);
		void StartTeleportInAnim(const Vec3& startWorldPos, const Vec3& targetWorldPos, double duration);

		void StopAnim();

		bool IsAnimating() const;

	private:
		PlayerAnimState mAnim;

		Model mCharacter;
};
