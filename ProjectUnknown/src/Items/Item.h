#pragma once

#include <string>

#include "ItemMeta.h"

enum ItemType : char;
enum ItemName : short;

class Item {
public:
	Item();
	Item(ItemName name);
	Item(ItemName name, int count);

	ItemName GetName();

	int count = 0;
	int maxCount = 50;

	ItemType type; // Set in itemhandler
	ItemName name;
	ItemMeta meta;
private:

};