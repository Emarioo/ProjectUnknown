#pragma once

#include "Items/ItemHandler.h"
#include "GameStateEnum.h"
#include "Objects/Player.h"

namespace game
{
	Player* GetPlayer();
	void SetPlayer(Player* player);
	/*
	Start game by setting the correct game state, lock the cursor, init items
	*/
	void InitGame();
	void Update(double delta);
	void Render(double lag);
}