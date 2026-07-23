#include "BlockGroup.hpp"

BlockGroup::BlockGroup(int32 id, GroupType type, const Array<Point>& tiles, Point pivot)
	: mID(id)
	, mType(type)
	, mTiles(tiles)
	, mPivot(pivot)
	, mRotationDir(1)
{

}

