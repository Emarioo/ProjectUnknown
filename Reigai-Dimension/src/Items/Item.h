#pragma once

#include <string>

class Item {
public:
	Item();
	Item(const std::string& name);
	Item(const std::string& name, int count);

	const std::string& GetName();

	int count = 0;
	int maxCount = 50;
	std::string name;
private:

};