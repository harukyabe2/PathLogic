#include "Player.hpp"
#include "Utils.hpp"

Player::Player()
	: mState(PlayerState::InUI)
	, mUIPos(100, 100)
	, mScreenPos(mUIPos)
	, mBoardPos(-1, -1)
	, mDirection(Direction::Up)
{

}

void Player::Update()
{
	// 初期位置とつかんでいる間は2Dで表示
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

void Player::ResetUIPos()
{
	mScreenPos = mUIPos;
}

