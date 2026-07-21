#include "StageLoader.hpp"

Board StageLoader::Load(FilePathView path)
{
	JSON json = JSON::Load(path);

	int32 width = json[U"width"].get<int32>();
	int32 height = json[U"height"].get<int32>();
	Board board(Size{ width, height });

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			board.SetTile({ x, y }, Tile(TileType::Empty, Direction::Up, -1));
		}
	}

	if (json.hasElement(U"tiles"))
	{
		for (const auto& tileJson : json[U"tiles"].arrayView())
		{
			int32 x = tileJson[U"x"].get<int32>();
			int32 y = tileJson[U"y"].get<int32>();
			
			String tileTypeStr = tileJson[U"type"].getString();
			TileType tileType = TileType::Normal;
			if (tileTypeStr == U"Arrow") tileType = TileType::Arrow;
			else if (tileTypeStr == U"Goal") tileType = TileType::Goal;
			
			String dirStr = tileJson[U"direction"].getString();
			Direction dir = Direction::Up;
			if (dirStr == U"Right") dir = Direction::Right;
			else if (dirStr == U"Down") dir = Direction::Down;
			else if (dirStr == U"Left") dir = Direction::Left;
			
			// グループに属さないため、groupIDは-1を指定する
			board.SetTile({ x, y }, Tile(tileType, dir, -1));
		}
	}

	for (const auto& groupJson : json[U"groups"].arrayView())
	{
		int32 id = groupJson[U"id"].get<int32>();
		
		String typeStr = groupJson[U"type"].getString();
		GroupType groupType = (typeStr == U"Rotate") ? GroupType::Rotate : GroupType::Slide;
		
		Point pivot{ groupJson[U"pivot"][U"x"].get<int32>(), groupJson[U"pivot"][U"y"].get<int32>() };

		Array<Point> tilePoints;

		for (const auto& tileJson : groupJson[U"tiles"].arrayView())
		{
			int32 x = tileJson[U"x"].get<int32>();
			int32 y = tileJson[U"y"].get<int32>();
			Point pos{ x, y };
			tilePoints.push_back(pos);

			String tileTypeStr = tileJson[U"type"].getString();
			TileType tileType = TileType::Normal;
			if (tileTypeStr == U"Arrow") tileType = TileType::Arrow;
			else if (tileTypeStr == U"Goal") tileType = TileType::Goal;

			String dirStr = tileJson[U"direction"].getString();
			Direction dir = Direction::Up;
			if (dirStr == U"Right") dir = Direction::Right;
			else if (dirStr == U"Down") dir = Direction::Down;
			else if (dirStr == U"Left") dir = Direction::Left;

			board.SetTile(pos, Tile(tileType, dir, id));
		}

		board.AddGroup(BlockGroup(id, groupType, tilePoints, pivot));
	}

	return board;
}
