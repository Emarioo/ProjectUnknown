#include "GameState.h"

#include <vector>

#include "DebugTool/DebugHandler.h"

namespace engine {
	
	bool gameStates[30];
	void SetState(unsigned char state,bool F) {
		if (state < MAX_CUSTOM_GAMESTATES + MAX_ENGINE_GAMESTATES) {
			gameStates[state] = F;
		}
	}
	bool CheckState(unsigned char state) {
		if (state < MAX_CUSTOM_GAMESTATES + MAX_ENGINE_GAMESTATES) {
			return gameStates[state];
		}
		return false;
	}
}