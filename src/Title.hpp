#pragma once
#include "Common.hpp"

class Title : public App::Scene
{
	public:
		Title(const InitData& init);

		void update() override;
		void draw() const override;
	private:
		RoundRect mStartButton{ Arg::center = Scene::Center().movedBy(0, 50), 250, 60, 5 };
		RoundRect mSelectButton{ Arg::center = Scene::Center().movedBy(0, 140), 250, 60, 5 };
};
