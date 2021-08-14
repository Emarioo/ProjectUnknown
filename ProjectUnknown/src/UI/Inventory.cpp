#include "propch.h"

#include "InterfaceManager.h"

#include "Items/ItemHandler.h"

#include "Keybindings.h"

Inventory::Inventory(const std::string& name) : engine::IBase(name) {
	container = new Container("Player Inv.",5,5);
	container->AddItem(new Item("Potato", 3));
	container->AddItem(new Item("Baked Potato", 1));
	container->AddItem(new Item("Butter", 3));
	container->AddItem(new Item("Raw Meat", 3));
	container->AddItem(new Item("Steak", 5));
	container->AddItem(new Item("Carrot", 8));
}
bool Inventory::MouseEvent(int mx, int my, int action, int button) {
	float mouseX = engine::ToFloatScreenX(mx);
	float mouseY = engine::ToFloatScreenY(my);

	if (action != 1)
		return false;

	// Check if mouse is inside inventory
	if (mouseX > x && mouseX<x + w &&
		mouseY>y && mouseY < y + h) {

		// If there is a container
		if (container != nullptr) {
			float sw = w * (64 / 440.f);
			float sh = h * (64 / 850.f);

			float startX = w * 63 / 440.f;
			float startY = -h * 241 / 850.f;

			float col = (mouseX - startX - x) / sw;
			float row = (startY + y + h - mouseY) / sh;

			//bug::outs <"stuff "< col < row < bug::end;

			if (0 <= row && row <= container->GetSlotWidth() && 0 <= col && col <= container->GetSlotHeight()) {

				Item** heldItem = interfaceManager.GetHeldItemPointer();
				Item** item = container->GetItemPointerAt((int)col, (int)row);

				if (item != nullptr) {
					container->SwitchItem(heldItem, item, button, action);
				}
			}
		}

	}

	return false;
}
bool Inventory::KeyEvent(int key, int action) {
	if (engine::TestActionKey(KeyInventory,key)) {
		if (action == 1) {
			active = !active;
			if (!interfaceManager.craftingList->active) {
				engine::LockCursor(!active);
			}
		}
	}
	return false;
}
void Inventory::Update(float delta) {
	
}
void Inventory::Render() {
	// Inventory background
	engine::BindTexture(0, "containers/inventory");

	engine::DrawRect(x, y, w, h, color.r, color.g, color.b, color.a);

	// Items
	if (container != nullptr) {
		float sw = w * (64 / 440.f);
		float sh = h * (64 / 850.f);

		float startX = w * 63 / 440.f;
		float startY = -h * 241 / 850.f;

		// slots start at pixel (63, 241) slot size 64
		for (int sy = 0; sy < container->GetSlotHeight(); sy++) {
			for (int sx = 0; sx < container->GetSlotWidth(); sx++) {
				Item* item = container->GetItemAt(sx, sy);

				if (item != nullptr) {
					// Draw item
					DrawItem(item,
						x + startX + sx * sw, y + h + startY + sy * -sh,
						sw, sh, 1, 1, 1, 1);
				}
			}
		}
	}
}