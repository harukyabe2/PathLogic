#include "Game.hpp"

Game::Game(const InitData& init)
	: IScene{ init }
	, mState(GameState::isPlaying)
	, mBackgroundColor(ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve())
	, mRenderTexture(Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes)
	, mCameraController(mRenderTexture.size())
	, mBoard({ 10, 10 })
	, mIsMouseRUp(false)
	, mIsMouseRDown(false)
{

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

		mBoard.Draw();

		Line3D{ Vec3{ 0, -5, -100 }, Vec3{ 0, -5, 100} }.draw();
		Line3D{ Vec3{ -100, -5, 0 }, Vec3{ 100, -5, 0 } }.draw();
	}

	Graphics3D::Flush();
	mRenderTexture.resolve();
	Shader::LinearToScreen(mRenderTexture);
}

void Game::ProcessInput()
{
	mIsMouseRUp = MouseR.up();
	mIsMouseRDown = MouseR.down();
}

void Game::UpdateGame()
{
	mCameraController.Update(mIsMouseRUp, mIsMouseRDown);
}
