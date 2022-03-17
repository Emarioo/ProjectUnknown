#include "gonpch.h"

#include "GameState.h"

#include "Logger.h"

namespace engone {
	
	EngoneHint operator|(EngoneHint a, EngoneHint b) {
		return (EngoneHint)(((uint32_t)a) | ((uint32_t)b));
	}
	bool operator==(EngoneHint a, EngoneHint b) {
		return (((uint32_t)a) & ((uint32_t)b)) > 0;
	}
	static EngoneHint engoneHints;
	EngoneHint GetEngoneHints() {
		return engoneHints;
	}
	void SetEngoneHints(EngoneHint hints) {
		engoneHints = hints;
	}

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