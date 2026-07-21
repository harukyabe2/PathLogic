#pragma once
#include <Siv3D.hpp>
#include "Board.hpp"

// アニメーションの状態を管理
struct GroupAnimState
{
	int32 groupID;
	double timer = 0.0;
	double duration = 0.25;

	Vec3 pivotWorldPos;
	double startAngle = 0.0;
	double endAngle = 0.0;
};

class BoardRenderer
{
	public:
		BoardRenderer();

		void Update();
		void Draw(const Board& board) const;

		void AddRotationAnim(int32 groupID, const Vec3& pivotWorldPos, double startAngle, double endAngle);

	private:
		void DrawSingleTile(const Tile& tile, const Vec3& pos, double angleOffset) const;

		Array<GroupAnimState> mAnims;

		Model mArrowBox;
};

