#pragma once
#include <Siv3D.hpp>
#include "Board.hpp"

struct StageData
{
	Board board;
	Direction playerDirection;
};

class StageLoader
{
	public:
		static StageData Load(FilePathView path);
};
