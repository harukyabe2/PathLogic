#include "StageSelect.hpp"

StageSelect::StageSelect(const InitData& init)
	: IScene{ init }
{
	int32 maxStage = getData().maxStage;

	Vec2 startPos = Scene::Center().movedBy(-((maxStage - 1) * 60), 0);

	for (int32 i = 0; i < maxStage; ++i)
	{
		mStageButtons.push_back(RoundRect{ Arg::center = startPos.movedBy(i * 120, 0), 80, 80, 10 });
	}
}

void StageSelect::update()
{
	if (mBackButton.leftClicked())
	{
		changeScene(U"Title");
	}

	for (size_t i = 0; i < mStageButtons.size(); ++i)
	{
		if (mStageButtons[i].leftClicked())
		{
			getData().currentStage = (int32)i + 1;
			changeScene(U"Game");
		}
	}
}

void StageSelect::draw() const
{
	Scene::SetBackground(Palette::Slategray);

	FontAsset(U"UI")(U"Select Stage").drawAt(Scene::Center().movedBy(0, -150), Palette::White);

	mBackButton.draw(Palette::Gray);
	FontAsset(U"ButtonUI")(U"Back").drawAt(mBackButton.center(), Palette::White).scaled(0.6);

	for (size_t i = 0; i < mStageButtons.size(); ++i)
	{
		mStageButtons[i].draw(Palette::Steelblue);

		FontAsset(U"UI")(Format(i + 1)).drawAt(mStageButtons[i].center(), Palette::White);
	}
}
