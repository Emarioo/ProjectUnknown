#include "Item.h"

namespace Inventory {
	Item::Item() {

	}
	const std::string& Item::GetName() {
		return name;
	}
	// Center? or top left/ bottom left
	void Item::Draw(float x, float y) {
		// Draw the image of the item.
		// Get image from a reference by item name
	}
}