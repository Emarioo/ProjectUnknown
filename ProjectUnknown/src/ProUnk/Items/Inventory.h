#pragma once

#include "Item.h"

namespace prounk {
	class Inventory {
	public:
		Inventory() = default;
		~Inventory();

		//-- Potential functions
		// transferItem
		// addItem
		// takeItem
		// sort name, count...
		// 

	private:

		std::vector<Item*> m_items; // all slots
	};
}