#pragma once
#include <Siv3D.hpp>
#include "Direction.hpp"

enum class TileType
{
	Empty,
	Normal,
	Arrow,
	Goal,
	Item,
	Teleport,
	RotateTrigger
};

class Tile
{
	public:
		Tile() = default;
		Tile(TileType type, Direction direction, int32 groupID);

		void ResetDirection();

		TileType GetType() const { return mType; }
		Direction GetDirection() const { return mDirection; }
		int32 GetGroupID() const { return mGroupID; }
		bool GetIsCollected() const { return mIsCollected; }
		int32 GetTeleportID() const { return mTeleportID; }

		void SetDirection(Direction dir) { mDirection = dir; }
		void SetDefaultDirection(Direction dir) { mDefaultDirection = dir; }
		void SetIsCollected(bool collected) { mIsCollected = collected; }
		void SetTeleportID(int32 id) { mTeleportID = id; }

	private:
		TileType mType;
		Direction mDirection;
		Direction mDefaultDirection;
		int32 mGroupID;

		bool mIsCollected;
		int32 mTeleportID;
};
