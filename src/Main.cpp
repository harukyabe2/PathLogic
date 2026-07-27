#include <Siv3D.hpp>
#include "Game.hpp"
#include "Title.hpp"
#include "StageSelect.hpp"
#include "Common.hpp"

bool Initialize();
void LoadData();

void Main()
{
	if (!Initialize()) return;

	App manager;
	manager.add<Title>(U"Title");
	manager.add<StageSelect>(U"StageSelect");
	manager.add<Game>(U"Game");

	while (System::Update())
	{
		if (not manager.update()) break;
	}
}

bool Initialize()
{
	Scene::Resize(1280, 720);
	Scene::SetResizeMode(ResizeMode::Keep);
	Window::SetStyle(WindowStyle::Sizable);
	Window::Maximize();
	Window::SetTitle(U"Shift Path Game");

	Graphics3D::SetGlobalAmbientColor(ColorF{ 0.75, 0.75, 0.75 });
	Graphics3D::SetSunColor(ColorF{ 0.5, 0.5, 0.5 });
	Graphics3D::SetSunDirection(Vec3{ 0, 1, -0.3 }.normalized());

	LoadData();

	return true;
}

void LoadData()
{
	FontAsset::Register(U"UI", FontMethod::MSDF, 48, Typeface::Bold);
	FontAsset::Register(U"ButtonUI", FontMethod::MSDF, 32, Typeface::Bold);

	TextureAsset::Register(U"ButtonArrow", Resource(U"imgs/right.png"));
	TextureAsset::Register(U"ButtonRetry", Resource(U"imgs/retry.png"));

	//TextureAsset::Register(U"", Resource(U"imgs/.png"));

	//AudioAsset::Register(U"", Resource(U"sounds/.wav"));

	//for (const auto& [name, info] : TextureAsset::Enumerate())
	//{
	//	TextureAsset::Load(name);
	//}

	//for (const auto& [name, info] : AudioAsset::Enumerate())
	//{
	//	AudioAsset::Load(name);
	//}
}

