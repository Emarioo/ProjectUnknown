#include "GameHandler.h"

#include "UI/InterfaceManager.h"

bool initGameAssets = false;
void GameHandler::StartGame() {
	engine::SetState(GameState::Menu, false);
	engine::LockCursor(true);
	engine::SetState(GameState::Game, true);
	if (!initGameAssets) {
		InitItemHandler();
		interfaceManager.SetupGameUI();
		initGameAssets = true;
	}
}
GameHandler gameHandler;