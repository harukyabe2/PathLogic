#pragma once
#include <Siv3D.hpp>

struct GameData
{
	int32 currentStage = 1;
	int32 maxStage = 4;
};

using App = SceneManager<String, GameData>;
