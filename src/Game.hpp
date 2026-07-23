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
	Result
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

		Player mPlayer;
		PlayerRenderer mPlayerRenderer;
		Point mPlayerStartPos;
		Direction mPlayerStartDir;
		Direction mDefaultPlayerDir;

		RoundRect mStartButton;
		RoundRect mRetryButton;

		bool mIsMouseRUp;
		bool mIsMouseRDown;
		bool mIsMouseLUp;
		bool mIsMouseLDown;
};



/*
設計についてのメモ
タイルは種類・方向・グループIDを持つ
ブロックグループは塊を作る
ボードはタイルと塊の管理をしつつ、回転・スライドに対応し、描画もする
ブロックグループは回転の軸や変換対象のタイル指定をするだけで、
ボード上での変換はすべてボードが対応
*/
