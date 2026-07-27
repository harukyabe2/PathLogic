#pragma once
#include "Common.hpp"

class StageSelect : public App::Scene
{
public:
	StageSelect(const InitData& init);

	void update() override;
	void draw() const override;

private:
	RoundRect mBackButton{ 30, 30, 120, 50, 5 };

	Array<RoundRect> mStageButtons;
};
