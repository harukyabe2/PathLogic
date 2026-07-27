#pragma once
#include <Siv3D.hpp>
#include "Common.hpp"
#include "CameraController.hpp"
#include "Board.hpp"
#include "BoardRenderer.hpp"
#include "StageLoader.hpp"
#include "Player.hpp"
#include "PlayerRenderer.hpp"

enum class GameState
{
	Editing,
	Simulating,
	Falling,
	TeleportingOut,
	TeleportingIn,
	GameOver,
	StageClear,
	GameClear
};

class Game : public App::Scene
{
	public:
		Game(const InitData& init);

		void update() override;
		void draw() const override;

	private:
		void ProcessInput();
		void UpdateGame();

		// 1マスずつ進めてアニメーションをセット
		void StepPlayer();

		GameState mState;

		const ColorF mBackgroundColor;
		const MSRenderTexture mRenderTexture;

		CameraController mCameraController;

		Board mBoard;
		BoardRenderer mBoardRenderer;

		int32 mTotalItems;
		int32 mCollectedItems;

		Player mPlayer;
		PlayerRenderer mPlayerRenderer;
		Point mPlayerStartPos;
		Direction mPlayerStartDir;
		Direction mDefaultPlayerDir;

		Point mNextTeleportPos;

		RoundRect mStartButton;
		RoundRect mRetryButton;
		RoundRect mNextButton;
		RoundRect mQuitButton;

		bool mIsMouseRUp;
		bool mIsMouseRDown;
		bool mIsMouseLUp;
		bool mIsMouseLDown;
};
