#include "GameState.h"

#include <vector>

namespace engine {
	
	std::vector<std::string> gameStates;
	void AddState(std::string state) {
		for (int i = 0; i < gameStates.size(); i++) {
			if (gameStates[i] == state) {
				return;
			}
		}
		gameStates.push_back(state);
	}
	void RemoveState(std::string state) {
		for (int i = 0; i < gameStates.size(); i++) {
			if (gameStates[i] == state) {
				gameStates.erase(gameStates.begin() + i);
				break;
			}
		}
	}
	bool CheckState(std::string state) {
		for (std::string str : gameStates) {
			if (str==state)
				return true;
		}
		return false;
	}
}