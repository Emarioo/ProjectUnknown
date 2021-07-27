#pragma once

#include "Items/ItemHandler.h"
#include "GameStateEnum.h"
#include "Objects/Player.h"

class GameHandler {
private:
public:
	Player* player=nullptr;

	void StartGame();
};
extern GameHandler gameHandler;