
#include "GameState.h"

#include "Logger.h"

namespace engone {
	
	EngoneOption operator|(EngoneOption a, EngoneOption b) {
		return (EngoneOption)(((uint32_t)a) | ((uint32_t)b));
	}
	//bool operator&(EngoneOption a, EngoneOption b) {
	//	return (((uint32_t)a) & ((uint32_t)b)) > 0;
	//}
	bool operator==(EngoneOption a, EngoneOption b) {
		return (((uint32_t)a) & ((uint32_t)b)) > 0;
	}
	static EngoneOption engoneOptions;
	EngoneOption GetEngoneOptions() {
		return engoneOptions;
	}
	bool HasEngoneOption(EngoneOption option) {
		return engoneOptions == option;
	}
	void SetEngoneOptions(EngoneOption options) {
		engoneOptions = options;
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