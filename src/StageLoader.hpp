#pragma once
#include <Siv3D.hpp>
#include "Board.hpp"

struct StageData
{
	Board board;
	Direction playerDirection;
	int32 totalItems;
};

class StageLoader
{
	public:
		static StageData Load(FilePathView path);
};
