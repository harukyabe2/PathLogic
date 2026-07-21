#include "Game.hpp"
#include "Utils.hpp"

Game::Game(const InitData& init)
	: IScene{ init }
	, mState(GameState::isPlaying)
	, mBackgroundColor(ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve())
	, mRenderTexture(Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes)
	, mCameraController(mRenderTexture.size())
	, mBoard({ 0, 0 })
	, mBoardRenderer()
	, mPlayerRenderer()
	, mIsMouseRUp(false)
	, mIsMouseRDown(false)
	, mIsMouseLUp(false)
	, mIsMouseLDown(false)
{
	mBoard = StageLoader::Load(U"stage.json");
}

void Game::update()
{
	ProcessInput();
	UpdateGame();
}

void Game::draw() const
{
	// 3D描画
	{
		const ScopedRenderTarget3D target{ mRenderTexture.clear(mBackgroundColor) };

		mBoardRenderer.Draw(mBoard);

		mPlayerRenderer.Draw3D(mPlayer, mBoard);

		Line3D{ Vec3{ 0, -5, -100 }, Vec3{ 0, -5, 100} }.draw();
		Line3D{ Vec3{ -100, -5, 0 }, Vec3{ 100, -5, 0 } }.draw();
	}

	Graphics3D::Flush();
	mRenderTexture.resolve();
	Shader::LinearToScreen(mRenderTexture);

	// 2D描画
	mPlayerRenderer.DrawUI(mPlayer);
}

void Game::ProcessInput()
{
	mIsMouseRUp = MouseR.up();
	mIsMouseRDown = MouseR.down();
	mIsMouseLUp = MouseL.up();
	mIsMouseLDown = MouseL.down();
}

void Game::UpdateGame()
{
	mCameraController.Update(mIsMouseRUp, mIsMouseRDown);

	mPlayer.Update(); 	

	const Ray ray = mCameraController.GetMouseRay();

	// つかんでいるプレイヤーを離したとき
	if (mPlayer.GetState() == PlayerState::Dragging && mIsMouseLUp)
	{
		bool isPlaced = false;

		if (auto hitPos = mBoard.RaycastTile(ray))
		{
			const Tile& tile = mBoard.GetTile(*hitPos);
			
			// マウスカーソルの指す位置が通常のタイルならそこに配置
			if (tile.GetType() == TileType::Normal)
			{
				mPlayer.SetBoardPos(*hitPos);
				mPlayer.SetDirection(tile.GetDirection());
				mPlayer.SetState(PlayerState::Placed);
				isPlaced = true;
			}
		}

		// 通常のタイル以外の位置なら元のUIの位置に戻す
		if (!isPlaced)
		{
			mPlayer.SetState(PlayerState::InUI);
			mPlayer.ResetUIPos();
		}
	}

	// プレイヤーをつかんでいない状態で左クリックしたとき
	if (mIsMouseLDown && mPlayer.GetState() != PlayerState::Dragging)
	{
		bool isPlayerPickedUp = false;

		// すでに置かれているプレイヤーをつかみ直す
		if (mPlayer.GetState() == PlayerState::Placed)
		{
			Vec3 playerWorldPos = mBoard.ToWorldPosition(mPlayer.GetBoardPos());
			Cylinder playerCollider{ playerWorldPos + Vec3{ 0, 0.5, 0 }, 0.4, 1.0 };

			if (ray.intersects(playerCollider))
			{
				mPlayer.SetState(PlayerState::Dragging);
				isPlayerPickedUp = true;
			}
		}

		if (!isPlayerPickedUp)
		{
			// 回転・移動に対応したブロックを左クリックしたとき
			if (auto clickedGroupID = mBoard.Raycast(ray))
			{
				Point pivot2D;
				bool isPlayerOnGroup = false;

				for (const auto& group : mBoard.GetGroups())
				{
					if (group.GetID() == *clickedGroupID)
					{
						pivot2D = group.GetPivot();

						for (const auto& pos : group.GetTiles())
						{
							if (pos == mPlayer.GetBoardPos())
							{
								isPlayerOnGroup = true;
								break;
							}
						}

						break;
					}
				}

				Vec3 pivot3D = mBoard.ToWorldPosition(pivot2D);

				// 回転・移動するブロック上にプレイヤーがいた場合は同時に動かす
				if (isPlayerOnGroup && mPlayer.GetState() == PlayerState::Placed)
				{
					Point playerPos = mPlayer.GetBoardPos();

					Point newPlayerPos = Utils::RotatePointRight(playerPos, pivot2D);

					Vec3 targetWorldPos = mBoard.ToWorldPosition(newPlayerPos);
					mPlayerRenderer.StartRotationAnim(pivot3D, targetWorldPos, -Math::HalfPi, 0.0);

					mPlayer.SetBoardPos(newPlayerPos);
					mPlayer.SetDirection(RotateRight(mPlayer.GetDirection()));
				}

				mBoard.RotateGroup(*clickedGroupID);
				mBoardRenderer.AddRotationAnim(*clickedGroupID, pivot3D, -Math::HalfPi, 0.0);
			}
		}
	}

	mBoardRenderer.Update();
	mPlayerRenderer.Update();
}
