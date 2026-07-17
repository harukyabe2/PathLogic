#include "Title.hpp"

Title::Title(const InitData& init)
	: IScene{ init }
{

}

void Title::update()
{
	if (MouseL.down())
	{
		changeScene(U"Game");
	}
}

void Title::draw() const
{
	Scene::SetBackground(Palette::Black);

	FontAsset(U"UI")(U"Shift Path").drawAt(Scene::Center(), Palette::White);
}
