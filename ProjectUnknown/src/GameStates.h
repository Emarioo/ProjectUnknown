#pragma once

enum GameState {
	Menu,
	Play
};
/*
Menu by default
*/
void SetGameState(GameState state);
/*
Menu by default
*/
bool IsGameState(GameState state);