#include "Game.hpp"
#include "Utils.hpp"

Game::Game(const InitData& init)
	: IScene{ init }
	, mState(GameState::Editing)
	, mBackgroundColor(Linear::Palette::Lightslategray)
	, mRenderTexture(Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes)
	, mCameraController(mRenderTexture.size())
	, mBoard({ 0, 0 })
	, mBoardRenderer()
	, mTotalItems(0)
	, mCollectedItems(0)
	, mPlayerRenderer()
	, mPlayerStartPos({ 0, 0 })
	, mPlayerStartDir(Direction::Up)
	, mDefaultPlayerDir(Direction::Up)
	, mNextTeleportPos({ 0, 0 })
	, mStartButton{ 1100, 600, 150, 100, 10 }
	, mRetryButton{ 930, 600, 150, 100, 10 }
	, mNextButton{ Arg::center = Scene::Center().movedBy(0, -50), 250, 60, 5 }
	, mQuitButton{ Arg::center = Scene::Center().movedBy(0, 40), 250, 60, 5 }
	, mIsMouseRUp(false)
	, mIsMouseRDown(false)
	, mIsMouseLUp(false)
	, mIsMouseLDown(false)
{
	// ステージ情報を受け取る
	int32 stageNum = getData().currentStage;
	String fileName = U"JSON/stage{}.json"_fmt(stageNum);
	StageData stageData = StageLoader::Load(fileName);
	mBoard = std::move(stageData.board);

	for (int y = 0; y < mBoard.GetSize().y; ++y)
	{
		for (int x = 0; x < mBoard.GetSize().x; ++x)
		{
			if (mBoard.GetTile({ x, y }).GetType() == TileType::Item) ++mTotalItems;
		}
	}

	mPlayerStartDir = stageData.playerDirection;
	mDefaultPlayerDir = mPlayerStartDir;
	mPlayer.SetDirection(mPlayerStartDir);
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
	{
		mPlayerRenderer.DrawUI(mPlayer);

		mStartButton.draw(Arg::top(0.6, 0.7, 0.8), Arg::bottom(0.3, 0.3, 0.4));
		TextureAsset(U"ButtonArrow").scaled(1.5, 0.8).draw(mStartButton.x + 5, mStartButton.y + 10);

		mRetryButton.draw(Arg::top(0.6, 0.7, 0.8), Arg::bottom(0.3, 0.3, 0.4));
		TextureAsset(U"ButtonRetry").draw(mRetryButton.x + 20, mRetryButton.y - 5);

		if (mState == GameState::StageClear)
		{
			Rect{ 1280, 720 }.draw(ColorF{ 0, 0.5 });

			mNextButton.draw(Arg::top(0.3, 0.4, 0.5), Arg::bottom(0.2, 0.2, 0.3));
			FontAsset(U"ButtonUI")(U"Next stage").drawAt(mNextButton.center(), Palette::White);

			mQuitButton.draw(Arg::top(0.3, 0.4, 0.5), Arg::bottom(0.2, 0.2, 0.3));
			FontAsset(U"ButtonUI")(U"Quit game").drawAt(mQuitButton.center(), Palette::White);
		}
		else if (mState == GameState::GameClear)
		{
			Rect{ 1280, 720 }.draw(ColorF{ 0, 0.5 });

			mQuitButton.movedBy(0, -40).draw(Arg::top(0.3, 0.4, 0.5), Arg::bottom(0.2, 0.2, 0.3));
			FontAsset(U"ButtonUI")(U"Quit game").drawAt(mQuitButton.center().movedBy(0, -40), Palette::White);
		}
	}
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

	double dt = Scene::DeltaTime();
	mBoardRenderer.Update(dt);
	mPlayerRenderer.Update(dt);

	// 配置フェーズ
	if (mState == GameState::Editing)
	{
		const Ray ray = mCameraController.GetMouseRay();
		const PlayerState playerState = mPlayer.GetState();

		// つかんでいるプレイヤーを離したとき
		if (playerState == PlayerState::Dragging && mIsMouseLUp)
		{
			bool isPlaced = false;

			if (auto hitPos = mBoard.RaycastTile(ray))
			{
				const Tile& tile = mBoard.GetTile(*hitPos);
				
				// マウスカーソルの指す位置がNormalのタイルならそこに配置
				if (tile.GetType() == TileType::Normal)
				{
					mPlayer.SetBoardPos(*hitPos);
					mPlayer.SetState(PlayerState::Placed);
					isPlaced = true;
				}
			}

			// Normalのタイル以外の位置なら元のUIの位置に戻す
			if (!isPlaced)
			{
				mPlayer.SetState(PlayerState::InUI);
				mPlayer.ResetUIPos();
			}
		}

		// プレイヤーをつかんでいない状態で左クリックしたとき
		if (mIsMouseLDown && playerState != PlayerState::Dragging)
		{
			bool isPlayerPickedUp = false;

			// すでに置かれているプレイヤーをつかみ直す
			if (playerState == PlayerState::Placed)
			{
				Vec3 playerWorldPos = mBoard.ToWorldPosition(mPlayer.GetBoardPos());
				Cylinder playerCollider{ playerWorldPos + Vec3{ 0, 0.5, 0 }, 0.4, 1.0 };

				if (ray.intersects(playerCollider))
				{
					mPlayer.SetState(PlayerState::Dragging);
					isPlayerPickedUp = true;
					mPlayer.SetDirection(mDefaultPlayerDir);
				}
			}

			if (!isPlayerPickedUp)
			{
				// 回転・移動に対応したブロックを左クリックしたとき
				if (auto clickedGroupID = mBoard.Raycast(ray))
				{
					Point pivot2D;
					bool isPlayerOnGroup = false;

					// 左クリックされたブロックのいずれかのタイルにプレイヤーが乗っているか確認
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

					int32 rotDir = mBoard.RotateGroup(*clickedGroupID);

					// 回転
					if (rotDir != 0)
					{
						double startAngle = (rotDir == 1) ? -Math::HalfPi : Math::HalfPi;

						// 回転するブロック上にプレイヤーがいた場合は同時に動かす
						if (isPlayerOnGroup && playerState == PlayerState::Placed)
						{
							Point playerPos = mPlayer.GetBoardPos();
							Direction playerDir = mPlayer.GetDirection();

							Point newPlayerPos = (rotDir == 1) ?
								Utils::RotatePointRight(playerPos, pivot2D)
								: Utils::RotatePointLeft(playerPos, pivot2D);

							Direction newPlayerDir = (rotDir == 1) ?
								RotateRight(playerDir)
								: RotateLeft(playerDir);

							mPlayer.SetDirection(newPlayerDir);

							Vec3 targetWorldPos = mBoard.ToWorldPosition(newPlayerPos);

							mPlayerRenderer.StartRotationAnim(pivot3D, targetWorldPos, startAngle, 0.0);

							mPlayer.SetBoardPos(newPlayerPos);
						}

						mBoardRenderer.AddRotationAnim(*clickedGroupID, pivot3D, startAngle, 0.0);
					}
					// スライド移動
					else
					{
						if (auto clickedGroupDir = mBoard.SlideGroup(*clickedGroupID))
						{
							// 移動するブロック上にプレイヤーがいた場合は同時に動かす
							if (isPlayerOnGroup && playerState == PlayerState::Placed)
							{
								Point playerPos = mPlayer.GetBoardPos();
								Vec3 starttWorldPos = mBoard.ToWorldPosition(playerPos);

								Point newPlayerPos = (playerPos + GetOffset(*clickedGroupDir));
								Vec3 targetWorldPos = mBoard.ToWorldPosition(newPlayerPos);

								mPlayerRenderer.StartWalkAnim(starttWorldPos, targetWorldPos, 0.25);

								mPlayer.SetBoardPos(newPlayerPos);
							}

							mBoardRenderer.AddSlideAnim(*clickedGroupID);
						}
					}
				}
			}
		}

		// プレイヤーが配置されて再生ボタンが押されたとき
		if (mState == GameState::Editing && playerState == PlayerState::Placed && mStartButton.leftClicked())
		{
			mState = GameState::Simulating;
			mPlayer.SetState(PlayerState::Walking);

			mPlayerStartPos = mPlayer.GetBoardPos();
			mPlayerStartDir = mPlayer.GetDirection();

			StepPlayer();
		}
	}
	// 再生フェーズ
	else if (mState == GameState::Simulating)
	{
		if (!mPlayerRenderer.IsAnimating())
		{
			Point currentPos = mPlayer.GetBoardPos();

			// タイルがないマスへ進んだ場合は落下処理
			if (!mBoard.IsInside(currentPos) || mBoard.GetTile(currentPos).GetType() == TileType::Empty)
			{
				Vec3 fallStart = mBoard.ToWorldPosition(currentPos);
				mPlayerRenderer.StartFallAnim(fallStart, 15.0, 1.0);

				mState = GameState::Falling;
			}
			else
			{
				const Tile& currentTile = mBoard.GetTile(currentPos);
				const TileType type = currentTile.GetType();

				// 現在のタイルがArrowなら方向を合わせる
				if (type == TileType::Arrow)
				{
					mPlayer.SetDirection(currentTile.GetDirection());
				}
				// 現在のタイルがItemならアイテム所持数を増やす
				else if (type == TileType::Item && !currentTile.GetIsCollected())
				{
					Tile& tile = mBoard.GetTile(currentPos);
					tile.SetIsCollected(true);
					++mCollectedItems;
				}
				// 現在のタイルがRotateTriggerならすべてのArrowタイルの向きを時計回りに90度回転させる
				else if (type == TileType::RotateTrigger)
				{
					mBoard.RotateAllArrowsRight();
				}
				else if (type == TileType::Teleport)
				{
					if (auto destPos = mBoard.FindPairedTeleport(currentTile.GetTeleportID(), currentPos))
					{
						mNextTeleportPos = *destPos;
						Vec3 startWorldPos = mBoard.ToWorldPosition(currentPos);

						mPlayerRenderer.StartTeleportOutAnim(startWorldPos, startWorldPos + Vec3{ 0, 10.0, 0 }, 0.5);
						mState = GameState::TeleportingOut;
						return;
					}
				}

				StepPlayer();
			}
		}
	}
	// 落下フェーズ
	else if (mState == GameState::Falling)
	{
		if (!mPlayerRenderer.IsAnimating())
		{
			mState = GameState::GameOver;
			mPlayer.SetState(PlayerState::Dead);
		}
	}
	// テレポートフェーズ：消える
	else if (mState == GameState::TeleportingOut)
	{
		if (!mPlayerRenderer.IsAnimating())
		{
			mPlayer.SetBoardPos(mNextTeleportPos);
			Vec3 destWorldPos = mBoard.ToWorldPosition(mNextTeleportPos);

			mPlayerRenderer.StartTeleportInAnim(destWorldPos + Vec3{ 0, 10.0, 0 }, destWorldPos, 0.5);
			mState = GameState::TeleportingIn;
		}
	}
	// テレポートフェーズ：現れる
	else if (mState == GameState::TeleportingIn)
	{
		if (!mPlayerRenderer.IsAnimating())
		{
			mState = GameState::Simulating;
			StepPlayer();
		}
	}
	// 結果フェーズ：失敗
	else if (mState == GameState::GameOver)
	{
		if (mRetryButton.leftClicked())
		{
			mState = GameState::Editing;

			mPlayerRenderer.StopAnim();

			// プレイヤーを落下する直前の位置と方向に戻す
			mPlayer.SetState(PlayerState::Placed);
			mPlayer.SetBoardPos(mPlayerStartPos);
			mPlayer.SetDirection(mPlayerStartDir);

			mCollectedItems = 0;

			mBoard.ResetBoardState();
		}
	}
	// 結果フェーズ：成功
	else if (mState == GameState::StageClear)
	{
		if (mNextButton.leftClicked())
		{
			changeScene(U"Game");
		}
		else if (mQuitButton.leftClicked())
		{
			changeScene(U"Title");
		}
	}
	else if (mState == GameState::GameClear)
	{
		if (mQuitButton.movedBy(0, -40).leftClicked())
		{
			changeScene(U"Title");
		}
	}
}

void Game::StepPlayer()
{
	Point currentPos = mPlayer.GetBoardPos();
	Direction currentDir = mPlayer.GetDirection();
	Point nextPos = currentPos + GetOffset(currentDir);

	Vec3 start3D = mBoard.ToWorldPosition(currentPos);
	Vec3 end3D = mBoard.ToWorldPosition(nextPos);

	mPlayerRenderer.StartWalkAnim(start3D, end3D, 0.4);

	mPlayer.SetBoardPos(nextPos);

	const Tile& nextTile = mBoard.GetTile(nextPos);
	TileType type = nextTile.GetType();

	if (type == TileType::Goal && (mCollectedItems >= mTotalItems))
	{
		int32 stageNum = ++getData().currentStage;
		if (stageNum <= getData().maxStage)
		{
			mState = GameState::StageClear;
		}
		else
		{
			mState = GameState::GameClear;
		}
	}
}
