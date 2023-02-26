#pragma once

#include "ProUnk/Registries/InventoryRegistry.h"

namespace prounk {
	void DrawItem(float x, float y, float size, Item* item);
	void DrawSlot(float x, float y, float size);
	bool MouseInsideSlot(float slotX, float slotY, float slotSize);

	// Draws a tooltip button which if hovered will display description.
	// The position of the tooltip description is calculated to fit the screen.
	// x,y and size is for the ? button.
	// textHeight is for the description
	void DrawToolTip(float x, float y, float size, float textHeight,const std::string& description);
	void DrawItemToolTip(float x, float y, float size, Item* item);
}