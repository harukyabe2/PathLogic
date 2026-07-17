#pragma once
#include <Siv3D.hpp>
#include "Tile.hpp"
#include "BlockGroup.hpp"

class Board
{
	public:
		Board(Size size);

		void Update();
		void Draw() const;

		void AddGroup(const BlockGroup& group);
		void RotateGroup(int32 id);
		void SlideGroup(int32 id, Direction dir);

		bool IsInside(const Point& point);

		Tile& GetTile(const Point& point) { return mTiles[point]; }
		void SetTile(Point point, const Tile& tile) { mTiles[point] = tile; }
		Size GetSize() const { return mTiles.size(); }

	private:
		Grid<Tile> mTiles;
		Array<BlockGroup> mGroups;
};
