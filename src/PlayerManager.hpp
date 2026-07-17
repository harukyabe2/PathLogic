#pragma once
#include <Siv3D.hpp>
#include "Board.hpp"
#include "Player.hpp"

class PlayerManager
{
	public:
		void Update(Board& board);

	private:
		Array<Player> mPlayers;
};
