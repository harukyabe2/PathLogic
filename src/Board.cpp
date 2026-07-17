#include "Board.hpp"

Board::Board(Size size)
	: mTiles(size)
{
	for (auto& tile : mTiles)
	{
		tile = Tile(TileType::Normal, Direction::Up, -1);
	}
}

void Board::Update()
{

}

void Board::Draw() const
{
	double blockSize = 1.5;
	double offsetX = (mTiles.width() - 1) * blockSize / 2.0;
	double offsetZ = (mTiles.height() - 1) * blockSize / 2.0;

	for (int y = 0; y < mTiles.height(); ++y)
	{
		for (int x = 0; x < mTiles.width(); ++x)
		{
			const Vec3 pos{
				x * blockSize - offsetX,
				2,
				y * blockSize - offsetZ
			};

			Box(pos, { 1.47, 0.3, 1.47 }).draw();
		}
	}
}

void Board::AddGroup(const BlockGroup& group)
{
	mGroups.push_back(group);
}

void Board::RotateGroup(int32 id)
{
	for (auto& group : mGroups)
	{

	}
}

void Board::SlideGroup(int32 id, Direction dir)
{
	for (auto& group : mGroups)
	{

	}
}

bool Board::IsInside(const Point& point)
{
	return mTiles.inBounds(point);
}
