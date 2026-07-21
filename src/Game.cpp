#include "Game.hpp"
#include "Utils.hpp"

Game::Game(const InitData& init)
	: IScene{ init }
	, mState(GameState::isPlaying)
	, mBackgroundColor(ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve())
	, mRenderTexture(Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes)
	, mCameraController(mRenderTexture.size())
	, mBoard({ 0, 0 })
	, mRenderer()
	, mPlayerRenderer()
	, mIsMouseRUp(false)
	, mIsMouseRDown(false)
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

		mRenderer.Draw(mBoard);

		if (mPlayer.GetState() == PlayerState::Placed || mPlayer.GetState() == PlayerState::Walking)
		{
			Vec3 playerWorldPos = mBoard.ToWorldPosition(mPlayer.GetBoardPos());
			mPlayerRenderer.Draw3D(mPlayer, mBoard);
		}

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
	mIsMouseLDown = MouseL.down();
}

void Game::UpdateGame()
{
	mCameraController.Update(mIsMouseRUp, mIsMouseRDown);

	mPlayer.Update(); 	

	const Ray ray = mCameraController.GetMouseRay();

	// プレイヤーをつかんでいる状態から離したとき、
	// 通常のタイルの位置ならそこに配置
	// 特殊なタイルの位置なら元の位置に戻す
	if (mPlayer.GetState() == PlayerState::Dragging && MouseL.up())
	{
		bool isPlaced = false;

		if (auto hitPos = mBoard.RaycastTile(ray))
		{
			const Tile& tile = mBoard.GetTile(*hitPos);
			
			if (tile.GetType() == TileType::Normal)
			{
				mPlayer.SetBoardPos(*hitPos);
				mPlayer.SetDirection(tile.GetDirection());
				mPlayer.SetState(PlayerState::Placed);
				isPlaced = true;
			}
		}

		if (!isPlaced)
		{
			mPlayer.SetState(PlayerState::InUI);
			mPlayer.ResetUIPos();
		}
	}

	if (mIsMouseLDown && mPlayer.GetState() != PlayerState::Dragging)
	{
		bool isPlayerPickedUp = false;

		// 配置してからの置き直し処理
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
			if (auto clickedGroupID = mBoard.Raycast(ray))
			{
				Point pivot2D;
				bool isPlayerOnGroup = false;

				for (const auto& group : mBoard.GetGroups()) {
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

				if (isPlayerOnGroup && mPlayer.GetState() == PlayerState::Placed)
				{
					Point playerPos = mPlayer.GetBoardPos();
					Vec3 pivot3D = mBoard.ToWorldPosition(pivot2D);

					Point newPlayerPos = Utils::RotatePointRight(playerPos, pivot2D);

					Vec3 targetWorldPos = mBoard.ToWorldPosition(newPlayerPos);
					mPlayerRenderer.StartRotationAnim(pivot3D, targetWorldPos, -Math::HalfPi, 0.0);

					mPlayer.SetBoardPos(newPlayerPos);
					mPlayer.SetDirection(RotateRight(mPlayer.GetDirection()));
				}

				Vec3 pivot3D = mBoard.ToWorldPosition(pivot2D);

				mBoard.RotateGroup(*clickedGroupID);
				mRenderer.AddRotationAnim(*clickedGroupID, pivot3D, -Math::HalfPi, 0.0);
			}
		}
	}

	mRenderer.Update();
	mPlayerRenderer.Update();
}
