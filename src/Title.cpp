#include "Title.hpp"

Title::Title(const InitData& init)
	: IScene{ init }
{

}

void Title::update()
{
	if (mStartButton.leftClicked())
	{
		getData().currentStage = 1;
		changeScene(U"Game");
	}
	else if (mSelectButton.leftClicked())
	{
		changeScene(U"StageSelect");
	}
}

void Title::draw() const
{
	Scene::SetBackground(Palette::Black);

	FontAsset(U"UI")(U"Path Logic").drawAt(Scene::Center().movedBy(0, -100), Palette::White);

	mStartButton.draw(Arg::top(0.3, 0.4, 0.5), Arg::bottom(0.2, 0.2, 0.3));
	FontAsset(U"ButtonUI")(U"Start").drawAt(mStartButton.center(), Palette::White);

	mSelectButton.draw(Arg::top(0.3, 0.4, 0.5), Arg::bottom(0.2, 0.2, 0.3));
	FontAsset(U"ButtonUI")(U"Stage Select").drawAt(mSelectButton.center(), Palette::White);
}

