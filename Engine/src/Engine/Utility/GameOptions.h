#pragma once

#include <map>
#include <string>
#include "../Handlers/FileHandler.h"

namespace engine {
	void ReadOptions();
	int GetOptioni(std::string);
	float GetOptionf(std::string);
	void AddOptioni(std::string, int);
	void AddOptionf(std::string, float);
}