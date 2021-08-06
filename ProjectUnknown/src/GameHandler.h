#pragma once

#include "Items/ItemHandler.h"
#include "GameStateEnum.h"
#include "Objects/Player.h"

class GameHandler {
private:
public:
	Player* player=nullptr;

	/*
	Start game by setting the correct game state, lock the cursor, init items
	*/
	void StartGame();
};
extern GameHandler gameHandler;