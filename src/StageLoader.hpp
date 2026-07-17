#pragma once
#include <Siv3D.hpp>
#include "Board.hpp"

class StageLoader
{
	public:
		static Board Load(FilePathView path);
};
