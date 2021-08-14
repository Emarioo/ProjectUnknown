#pragma once

#define MAX_ENGINE_GAMESTATES 5
#define MAX_CUSTOM_GAMESTATES 15

/*
This is the standard value for custom gamestates.
#define MAX_CUSTOM_GAMESTATES 15
*/
namespace GameState {
	enum EngineGameState : unsigned char {
		DebugLog = MAX_CUSTOM_GAMESTATES,
		DebugMode
	};
}

namespace engine {
	void SetState(unsigned char state,bool f);
	bool CheckState(unsigned char state);
}