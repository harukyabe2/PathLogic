#pragma once
#include <Siv3D.hpp>
#include "Player.hpp"
#include "Board.hpp"

// アニメーションの状態を管理
struct PlayerAnimState
{
	bool isAnimating = false;

	double timer = 0.0;
	double duration = 0.25;

	Vec3 pivotWorldPos;
	Vec3 targetWorldPos;

	double startAngle = 0.0;
	double endAngle = 0.0;
};

class PlayerRenderer
{
	public:
		void Update();
		void DrawUI(const Player& player) const;
		void Draw3D(const Player& player, const Board& board) const;

		void StartRotationAnim(const Vec3& pivotWorldPos, const Vec3& targetWorldPos, double startAngle, double endAngle);

	private:
		PlayerAnimState mAnim;
};
