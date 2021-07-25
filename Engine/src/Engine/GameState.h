#pragma once

#include <string>

#define MAX_ENGINE_GAMESTATES 5
#define MAX_CUSTOM_GAMESTATES 25

namespace engine {
	void SetState(unsigned char state,bool f);
	bool CheckState(unsigned char state);

	enum EngineGameState : unsigned char {
		Debug = MAX_CUSTOM_GAMESTATES
	};
}