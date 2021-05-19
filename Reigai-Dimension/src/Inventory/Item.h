#pragma once

#include <string>
#include "Engine/Engine.h"

class Item {
public:
	Item();

	const std::string& GetName();

	void Draw(float x, float y);

	int count;
	std::string name;
private:

};