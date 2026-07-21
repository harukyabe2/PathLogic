#pragma once
#include <Siv3D.hpp>
#include "Common.hpp"
#include "CameraController.hpp"
#include "Board.hpp"
#include "BoardRenderer.hpp"
#include "StageLoader.hpp"
#include "Player.hpp"

enum class GameState
{
	isPlaying,
	isPaused,
	isGameClear
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

		GameState mState;

		const ColorF mBackgroundColor;
		const MSRenderTexture mRenderTexture;

		CameraController mCameraController;

		Board mBoard;
		BoardRenderer mRenderer;

		Player mPlayer;

		bool mIsMouseRUp;
		bool mIsMouseRDown;
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
