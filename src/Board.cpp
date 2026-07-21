#include "Board.hpp"
#include "Utils.hpp"

Board::Board(Size size)
	: mTiles(size)
{

}

void Board::Update()
{

}

void Board::AddGroup(const BlockGroup& group)
{
	mGroups.push_back(group);
}

void Board::RotateGroup(int32 id)
{
	for (auto& group : mGroups)
	{
		if (group.GetID() == id)
		{
			if (group.GetType() != GroupType::Rotate) return;

			const Point pivot = group.GetPivot();
			const Array<Point>& currentPositions = group.GetTiles();
			Array<Point> nextPositions;

			// 回転後の座標を計算
			for (const auto& pos : currentPositions)
			{
				nextPositions.push_back(Utils::RotatePointRight(pos, pivot));
			}

			// 衝突判定（回転可能か）
			bool canRotate = true;
			for (const auto& nextPos : nextPositions)
			{
				if (!IsInside(nextPos))
				{
					canRotate = false;
					break;
				}

				// 移動先に自身のブロックを構成するタイル以外のものがあるか確認
				int32 targetGroupID = mTiles[nextPos].GetGroupID();
				if (targetGroupID != -1 && targetGroupID != id)
				{
					canRotate = false;
					break;
				}
			}

			if (!canRotate) return;

			Array<Tile> tempTiles;

			// 現在のマス目からブロックをいったん消去
			for (const auto& pos : currentPositions)
			{
				tempTiles.push_back(mTiles[pos]);
				mTiles[pos] = Tile(TileType::Empty, Direction::Up, -1);
			}

			group.SetTiles(nextPositions);

			// 新しいマス目にデータを書き込み、タイルのDirectionも回転させる
			for (size_t i = 0; i < nextPositions.size(); ++i)
			{
				const Point& pos = nextPositions[i];
				Tile& tile = tempTiles[i];

				tile.SetDirection(RotateRight(tile.GetDirection()));
				mTiles[pos] = tile;
			}

			break;
		}
	}
}

void Board::SlideGroup(int32 id, Direction dir)
{
	for (auto& group : mGroups)
	{

	}
}

bool Board::IsInside(const Point& point) const
{
	return mTiles.inBounds(point);
}

Vec3 Board::ToWorldPosition(const Point& point) const
{
	constexpr double blockSize = 1.5;

	double offsetX = (mTiles.width() - 1) * blockSize / 2.0;
	double offsetZ = (mTiles.height() - 1) * blockSize / 2.0;

	return {
			point.x * blockSize - offsetX,
			2,
			point.y * blockSize - offsetZ
	};
}

Optional<Point> Board::RaycastTile(const Ray& ray) const
{
	Optional<Point> hitPos;
	double minDistance = Math::Inf;

	for (int y = 0; y < mTiles.height(); ++y)
	{
		for (int x = 0; x < mTiles.width(); ++x)
		{
			const Tile& tile = mTiles[y][x];

			if (tile.GetType() == TileType::Empty) continue;

			const Box box{ ToWorldPosition({ x, y }), { 1.47, 0.3, 1.47 } };

			if (auto dist = ray.intersects(box))
			{
				if (*dist < minDistance)
				{
					minDistance = *dist;
					hitPos = Point{ x, y };
				}
			}
		}
	}

	return hitPos;
}

Optional<int32> Board::Raycast(const Ray& ray) const
{
	if (auto hitPos = RaycastTile(ray))
	{
		int32 groupID = mTiles[*hitPos].GetGroupID();
		if (groupID != -1)
		{
			return groupID;
		}
	}

	return none;
}
