#pragma once

#include "ProUnk/DataRegistries/InventoryRegistry.h"

namespace prounk {
	class World;
	void DrawItem(float x, float y, float size, Item item);
	void DrawSlot(float x, float y, float size);
	bool MouseInsideSlot(float slotX, float slotY, float slotSize);
}