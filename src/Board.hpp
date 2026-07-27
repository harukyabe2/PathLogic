#pragma once
#include <Siv3D.hpp>
#include "Tile.hpp"
#include "BlockGroup.hpp"

class Board
{
	public:
		Board(Size size);

		void Update();

		void ResetBoardState();

		void AddGroup(const BlockGroup& group);
		int32 RotateGroup(int32 id);
		Optional<Direction> SlideGroup(int32 id);

		bool IsInside(const Point& point) const;

		Vec3 ToWorldPosition(const Point& point) const;

		// マウスカーソルの位置にあるタイルの座標を返す関数
		Optional<Point> RaycastTile(const Ray& ray) const;
		// マウスカーソルの位置にあるタイルのグループIDを返す関数
		Optional<int32> Raycast(const Ray& ray) const;

		bool CheckRotation(int32 id, const Point& pivot, int32 dir, const Array<Point>& currentPositions, Array<Point>& nextPositions);
		bool CheckSlide(int32 id, Direction dir, const Array<Point>& currentPositions, Array<Point>& nextPositions);

		void RotateAllArrowsRight();

		Optional<Point> FindPairedTeleport(int32 id, const Point& excludePos) const;

		const Tile& GetTile(const Point& point) const { return mTiles[point]; }
		Tile& GetTile(const Point& point) { return mTiles[point]; }
		const Size GetSize() const { return mTiles.size(); }
		const Array<BlockGroup>& GetGroups() const { return mGroups; }

		void SetTile(Point point, const Tile& tile) { mTiles[point] = tile; }

	private:
		Grid<Tile> mTiles;
		Array<BlockGroup> mGroups;
};
