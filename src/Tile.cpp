#include "Tile.hpp"

Tile::Tile(TileType type, Direction direction = Direction::Up, int32 groupID = -1)
	: mType(type)
	, mDirection(direction)
	, mGroupID(groupID)
{

}

