#pragma once

#include <string>
#include <vector>
#include "Utility/FileHandler.h"

class PlayerData {
public:
	PlayerData(std::string data);

	std::string name;
	std::string world;
	// skills, items...

	std::string GetData();

	void LoadData(std::string data);
};