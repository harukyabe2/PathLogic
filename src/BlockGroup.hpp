#pragma once
#include <Siv3D.hpp>
#include "Direction.hpp"

enum class GroupType
{
	Rotate,
	Slide
};

class BlockGroup
{
	public:
		BlockGroup(int32 id, GroupType type, const Array<Point>& tiles, Point pivot);

		int32 GetID() const { return mID; }
		GroupType GetType() const { return mType; }
		const Array<Point>& GetTiles() const { return mTiles; }
		Point GetPivot() const { return mPivot; }
		int32 GetRotationDirection() const { return mRotationDir; }
		Direction GetSlideDirection() const { return mSlideDir; }

		void SetTiles(const Array<Point>& tiles) { mTiles = tiles; }
		void SetPivot(const Point& pivot) { mPivot = pivot; }
		void SetRotationDirection(int32 dir) { mRotationDir = dir; }
		void SetSlideDirection(Direction dir) { mSlideDir = dir; }

	private:
		int32 mID;
		GroupType mType;
		Array<Point> mTiles;
		Point mPivot;

		// 1なら時計回り、-1なら反時計回り
		int32 mRotationDir;
		// Upならy軸正
		Direction mSlideDir;
};
