#include "propch.h"

#include "GameHandler.h"

#include "UI/InterfaceManager.h"

bool initGameAssets = false;
void GameHandler::StartGame() {
	engone::SetState(GameState::Menu, false);
	engone::renderer->LockCursor(true);
	engone::SetState(GameState::Game, true);
	if (!initGameAssets) {
		InitItemHandler();
		//interfaceManager.SetupGameUI();
		initGameAssets = true;
	}
}
GameHandler gameHandler;