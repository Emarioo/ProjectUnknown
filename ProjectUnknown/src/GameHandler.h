#pragma once

#include "Items/ItemHandler.h"
#include "GameStateEnum.h"
#include "Objects/Player.h"

namespace game
{
	/*
	Start game by setting the correct game state, lock the cursor, init items
	*/
	void StartGame();
	Player* GetPlayer();
	void SetPlayer(Player* player);
}