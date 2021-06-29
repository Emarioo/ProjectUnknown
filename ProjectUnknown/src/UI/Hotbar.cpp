#include "Hotbar.h"

#include "GLFW/glfw3.h"

#include "Items/ItemHandler.h"

#include "InterfaceManager.h"

Hotbar::Hotbar(const std::string& name) : engine::IBase(name) {
	active = true;
	
	container = new Container("Player Hotbar", 9, 1);
	container->AddItem(new Item(FiberItem, 23));
	container->AddItem(new Item(FlintItem, 10));
	container->AddItem(new Item(StickItem, 50));
	container->AddItem(new Item(FlintItem, 250));
	
}
bool Hotbar::IsActive() {
	return active;
}
bool Hotbar::ClickEvent(int mx, int my, int button, int action) {
	CalcConstraints();

	float mouseX = engine::ToFloatScreenX(mx);
	float mouseY = engine::ToFloatScreenY(my);

	// Check if mouse is inside inventory
	if (mouseX > x - w / 2 && mouseX<x + w / 2 &&
		mouseY>y - h / 2 && mouseY < y + h / 2) {
		
		// If there is a container
		if (container != nullptr) {
			float sw = w * (64 / 608.f);
			float sh = h;// *(64 / 64.f);

			float gap = w * 4 / 608.f;

			float row = (mouseX - x + w / 2) / (sw+gap);
			float col = (mouseY - y - h / 2) / -sh;

			//bug::outs < row < col < bug::end;

			float rawRow = (row - (int)row) * (sw + gap); // Ignore gap

			if (rawRow < sw && row > 0 && col > 0 && row < container->GetSlotWidth() && col < container->GetSlotHeight()) {
				
				Item** heldItem = UI::GetHeldItemPointer();
				Item** item = container->GetItemPointerAt((int)row, (int)col);

				if (item != nullptr) {
					container->SwitchItem(heldItem, item, button, action);
				}
			}
		}
	}

	return false;
}
void Hotbar::Update(float delta) {
	if (IsActive()) {

	}
}
void Hotbar::Render() {
	//bug::out < "Invetory Draw" < bug::end;
	if (IsActive()) {
		// Inventory background
		engine::BindTexture(0, "containers/hotbar");

		CalcConstraints();

		engine::DrawRect(x, y, w, h, color.r, color.g, color.b, color.a);

		// Items
		if (container != nullptr) {
			float wid = engine::Width();
			float hei = engine::Height();

			float gap = w * (4 / 608.f);

			float sw = w * (64 / 608.f);
			float sh = h * (64 / 64.f);

			for (int sx = 0; sx < container->GetSlotWidth(); sx++) {
				Item* item = container->GetItemAt(sx, 0);

				if (item != nullptr) {
					// Draw item
					DrawItem(item,
						x - w / 2 + sx * (sw+gap), y + h / 2,
						sw, sh);
				}
			}
		}
	}
}