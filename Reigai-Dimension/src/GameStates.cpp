#include "GameStates.h"

#include "Engine/Engine.h"

GameState globalGameState = GameState::Menu;
void SetGameState(GameState state) {
	if (globalGameState == state)
		return;
	if (state == Menu) {
		engine::SetPauseMode(true);
	} else if (state == Play) {
		engine::SetPauseMode(false);
	}
	globalGameState = state;
}
bool IsGameState(GameState state) {
	return globalGameState == state;
}