#pragma once

#include "../Handlers/FileHandler.h"

namespace engone {
	void ReadOptions();
	int GetOptioni(std::string);
	float GetOptionf(std::string);
	void AddOptioni(std::string, int);
	void AddOptionf(std::string, float);
}