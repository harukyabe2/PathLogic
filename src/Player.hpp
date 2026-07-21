#pragma once
#include <Siv3D.hpp>
#include "Direction.hpp"

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
		void ResetUIPos();

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

		Vec2 mUIPos;
		Vec2 mScreenPos;

		Point mBoardPos;
		Direction mDirection;
};
