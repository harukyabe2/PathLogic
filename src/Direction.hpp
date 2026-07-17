#pragma once

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
