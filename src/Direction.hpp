#pragma once
#include <Siv3D.hpp>

enum class Direction
{
	Up,
	Right,
	Down,
	Left
};

Direction RotateRight(Direction dir);
Direction RotateLeft(Direction dir);

Point GetOffset(Direction dir);

double GetBaseAngle(Direction dir);
