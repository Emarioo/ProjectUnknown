#pragma once

#include "ItemType.h"

#ifdef gone
class Item {
public:
	Item();
	Item(const std::string& name);
	Item(const std::string& name, int count);

	std::string GetName();

	void SetItem(const std::string& name,int count);

	int count = 0;

	ItemType type;

};
#endif