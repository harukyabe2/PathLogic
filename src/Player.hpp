#pragma once
#include <Siv3D.hpp>
#include "Direction.hpp"

class Player
{
	public:
		Player(Point pos, Direction dir);

		void Update();

	private:
		Point mPos;
		Direction mDir;
};
