#pragma once

#include <string>

class WorldItem {
public:
	std::string name, id;
	WorldItem(std::string s, std::string i) {
		name = s;
		id = i;
	}
};