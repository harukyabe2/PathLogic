#include "Board.hpp"
#include "Utils.hpp"

Board::Board(Size size)
	: mTiles(size)
{

}

void Board::Update()
{

}

void Board::ResetBoardState()
{
	Size size = GetSize();
	for (int y = 0; y < size.y; ++y)
	{
		for (int x = 0; x < size.x; ++x)
		{
			Tile& tile = GetTile({ x, y });
			TileType type = tile.GetType();

			if (type == TileType::Arrow) tile.ResetDirection();
			if (type == TileType::Item) tile.SetIsCollected(false);
		}
	}
}

void Board::AddGroup(const BlockGroup& group)
{
	mGroups.push_back(group);
}

int32 Board::RotateGroup(int32 id)
{
	for (auto& group : mGroups)
	{
		if (group.GetID() == id)
		{
			if (group.GetType() != GroupType::Rotate) return 0;

			const Point pivot = group.GetPivot();
			const Array<Point>& currentPositions = group.GetTiles();
			int32 currentDir = group.GetRotationDirection();

			Array<Point> nextPositions;

			// 時計回りで回転できるかを確認し、できない場合は反時計回りでも確認する
			if (!CheckRotation(id, pivot, currentDir, currentPositions, nextPositions))
			{
				currentDir *= -1;
				if (!CheckRotation(id, pivot, currentDir, currentPositions, nextPositions)) return 0;
				group.SetRotationDirection(currentDir);
			}

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

				Direction newDir = (currentDir == 1) ?
					RotateRight(tile.GetDirection()) :
					RotateLeft(tile.GetDirection());

				tile.SetDirection(newDir);
				tile.SetDefaultDirection(newDir);

				mTiles[pos] = tile;
			}

			return currentDir;
		}
	}

	return 0;
}

Optional<Direction> Board::SlideGroup(int32 id)
{
	for (auto& group : mGroups)
	{
		if (group.GetID() == id)
		{
			if (group.GetType() != GroupType::Slide) return none;

			const Array<Point>& currentPositions = group.GetTiles();
			Direction currentDir = group.GetSlideDirection();

			Array<Point> nextPositions;

			// 既定の方向に移動できるか確認し、できない場合は反対方向でも確認する
			if (!CheckSlide(id, currentDir, currentPositions, nextPositions))
			{
				currentDir = Reverse(currentDir);
				if (!CheckSlide(id, currentDir, currentPositions, nextPositions)) return none;
				group.SetSlideDirection(currentDir);
			}

			Array<Tile> tempTiles;

			// 現在のマス目からブロックをいったん消去
			for (const auto& pos : currentPositions)
			{
				tempTiles.push_back(mTiles[pos]);
				mTiles[pos] = Tile(TileType::Empty, Direction::Up, -1);
			}

			group.SetTiles(nextPositions);

			// 新しいマス目にデータを書き込む
			for (size_t i = 0; i < nextPositions.size(); ++i)
			{
				const Point& pos = nextPositions[i];
				Tile& tile = tempTiles[i];

				mTiles[pos] = tile;
			}

			return currentDir;
		}
	}

	return none;
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
			offsetZ - point.y * blockSize 
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

bool Board::CheckRotation(int32 id, const Point& pivot, int32 dir, const Array<Point>& currentPositions, Array<Point>& nextPositions)
{
	nextPositions.clear();
	bool canRotate = true;

	for (size_t i = 0; i < currentPositions.size(); ++i)
	{
		const Point& pos = currentPositions[i];

		// 回転の向きによって移動後の位置を計算
		Point nextPos = (dir == 1) ?
			Utils::RotatePointRight(pos, pivot)
			: Utils::RotatePointLeft(pos, pivot);

		if (!IsInside(nextPos))
		{
			canRotate = false;
			break;
		}

		const Tile& targetTile = mTiles[nextPos];
		int32 targetGroupID = targetTile.GetGroupID();
		TileType targetType = targetTile.GetType();

		// 移動先に自身のブロックを構成するタイル以外のものがあるか確認
		if (targetGroupID != id && targetType != TileType::Empty)
		{
			canRotate = false;
			break;
		}

		// 現在の位置で隣接するタイルが自身のブロックを構成する以外のものなら回転を不可にする
		if (Abs(pos.x - nextPos.x) == 1 && Abs(pos.y - nextPos.y) == 1)
		{
			Point corner1{ pos.x, nextPos.y };
			Point corner2{ nextPos.x, pos.y };
			
			auto checkSweep = [&](const Point& p) {
			
			if (!IsInside(p)) return true;
			
			const Tile& t = mTiles[p];
			
			return (t.GetGroupID() == id || t.GetType() == TileType::Empty);
			
			};
			
			if (!checkSweep(corner1) || !checkSweep(corner2))
			{
				canRotate = false;
				break;
			}
		}

		nextPositions.push_back(nextPos);
	}

	return canRotate;
}

bool Board::CheckSlide(int32 id, Direction dir, const Array<Point>& currentPositions, Array<Point>& nextPositions)
{
	nextPositions.clear();
	bool canSlide = true;

	for (size_t i = 0; i < currentPositions.size(); ++i)
	{
		const Point& pos = currentPositions[i];

		// 移動の向きによって移動後の位置を計算
		Point nextPos = (pos + GetOffset(dir));

		if (!IsInside(nextPos))
		{
			canSlide = false;
			break;
		}

		const Tile& targetTile = mTiles[nextPos];
		int32 targetGroupID = targetTile.GetGroupID();
		TileType targetType = targetTile.GetType();

		// 移動先に自身のブロックを構成するタイル以外のものがあるか確認
		if (targetGroupID != id && targetType != TileType::Empty)
		{
			canSlide = false;
			break;
		}

		nextPositions.push_back(nextPos);
	}

	return canSlide;
}

void Board::RotateAllArrowsRight()
{
	Size size = GetSize();
	for (int y = 0; y < size.y; ++y)
	{
		for (int x = 0; x < size.x; ++x)
		{
			Tile& tile = GetTile({ x, y });

			if (tile.GetType() == TileType::Arrow)
			{
				tile.SetDirection(RotateRight(tile.GetDirection()));
			}
		}
	}
}

Optional<Point> Board::FindPairedTeleport(int32 id, const Point& excludePos) const
{
	Size size = GetSize();
	for (int y = 0; y < size.y; ++y)
	{
		for (int x = 0; x < size.x; ++x)
		{
			Point checkPos{ x, y };
			const Tile& tile = GetTile(checkPos);

			if (tile.GetType() == TileType::Teleport &&
				tile.GetTeleportID() == id &&
				checkPos != excludePos)
			{
				return checkPos;
			}
		}
	}

	return none;
}

