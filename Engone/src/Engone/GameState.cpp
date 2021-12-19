#include "gonpch.h"

#include "GameState.h"

#include "DebugHandler.h"

namespace engone {
	
	bool gameStates[MAX_CUSTOM_GAMESTATES+MAX_ENGINE_GAMESTATES];
	void SetState(unsigned char state, bool F) {
		if (state < MAX_CUSTOM_GAMESTATES + MAX_ENGINE_GAMESTATES) {
			gameStates[state] = F;
		}
		else {
			log::out << log::RED << "Invalid state: " << state << "\n" << log::WHITE;
		}
	}
	bool CheckState(unsigned char state) {
		if (state < MAX_CUSTOM_GAMESTATES + MAX_ENGINE_GAMESTATES) {
			return gameStates[state];
		}
		else {
			log::out << log::RED << "Invalid state: " << state << "\n"<<log::WHITE;
		}
		return false;
	}
}