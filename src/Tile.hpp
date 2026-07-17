#pragma once
#include <Siv3D.hpp>
#include "Direction.hpp"

enum class TileType
{
	Empty,
	Normal,
	Arrow,
	Teleport,
	Goal,
	RotateTrigger
};

class Tile
{
	public:
		Tile() = default;
		Tile(TileType type, Direction direction, int32 groupID);

		TileType GetType() const { return mType; }
		Direction GetDirection() const { return mDirection; }
		int32 GetGroupID() const { return mGroupID; }

		void SetDirection(Direction dir) { mDirection = dir; }

	private:
		TileType mType;
		Direction mDirection;
		int32 mGroupID;
};
