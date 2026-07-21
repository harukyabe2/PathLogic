#include "Direction.hpp"

Direction RotateRight(Direction dir)
{
	return static_cast<Direction>((static_cast<int>(dir) + 1) % 4);
}

Direction RotateLeft(Direction dir)
{
	return static_cast<Direction>((static_cast<int>(dir) + 3) % 4);
}

Point GetOffset(Direction dir)
{
	switch (dir)
	{
		case Direction::Up:
			return { 0, -1 };
		case Direction::Right:
			return { 1, 0 };
		case Direction::Down:
			return { 0, 1 };
		case Direction::Left:
			return { -1, 0 };
	}

	return { 0, 0 };
}

double GetBaseAngle(Direction dir)
{
	switch (dir)
	{
		case Direction::Up: return 0.0;
		case Direction::Right: return Math::HalfPi;
		case Direction::Down: return Math::Pi;
		case Direction::Left: return -Math::HalfPi;
	}

	return 0.0;
}
