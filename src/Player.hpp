#pragma once
#include <Siv3D.hpp>
#include "Direction.hpp"

// アニメーションの状態を管理
struct PlayerAnimState
{
	bool isAnimating = false;

	double timer = 0.0;
	double duration = 0.25;

	Vec3 pivotWorldPos;
	Vec3 targetWorldPos;

	double startAngle = 0.0;
	double endAngle = 0.0;
};

enum class PlayerState
{
	InUI,
	Dragging,
	Placed,
	Walking
};

class Player
{
	public:
		Player();

		void Update();
		void DrawUI() const;
		void Draw3D(const Vec3& worldPos) const;
		void ResetUIPos();

		void StartRotationAnim(const Vec3& pivotWorldPos, const Vec3& targetWorldPos, double startAngle, double endAngle);

		PlayerState GetState() const { return mState; }
		Vec2 GetScreenPos() const { return mScreenPos; }
		Point GetBoardPos() const { return mBoardPos; }
		Direction GetDirection() const { return mDirection; }

		void SetState(PlayerState state) { mState = state; }
		void SetScreenPos(const Vec2& pos) { mScreenPos = pos; }
		void SetBoardPos(const Point& pos) { mBoardPos = pos; }
		void SetDirection(Direction dir) { mDirection = dir; }

	private:
		PlayerState mState;

		PlayerAnimState mAnim;

		Vec2 mUIPos;
		Vec2 mScreenPos;

		Point mBoardPos;
		Direction mDirection;
};
