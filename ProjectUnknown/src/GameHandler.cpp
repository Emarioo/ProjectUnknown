#include "gonpch.h"

#include "GameHandler.h"

#include "UI/InterfaceManager.h"

namespace game
{
	static bool initGameAssets = false;
	void StartGame()
	{
		engone::SetState(GameState::Menu, false);
		engone::LockCursor(true);
		engone::SetState(GameState::Game, true);
		if (!initGameAssets) {
			InitItemHandler();
			//interfaceManager.SetupGameUI();
			initGameAssets = true;
		}
	}
	static Player* _player;
	void SetPlayer(Player* player)
	{
		_player = player;
	}
	Player* GetPlayer()
	{
		return _player;
	}
}