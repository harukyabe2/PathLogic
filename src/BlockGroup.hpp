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

		void SetTiles(const Array<Point>& tiles);
		void SetPivot(const Point& pivot);

	private:
		int32 mID;
		GroupType mType;
		Array<Point> mTiles;
		Point mPivot;
};
