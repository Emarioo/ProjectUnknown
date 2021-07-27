#include "Hotbar.h"

#include "GLFW/glfw3.h"

#include "Items/ItemHandler.h"

#include "InterfaceManager.h"

Hotbar::Hotbar(const std::string& name) : engine::IBase(name) {
	active = true;
	
	container = new Container("Player Hotbar", 9, 1);
	/*container->AddItem(new Item("Fiber", 23));
	container->AddItem(new Item("Flint", 8));
	container->AddItem(new Item("Stick", 35));
	container->AddItem(new Item("Flint", 1));*/
	
}
bool Hotbar::MouseEvent(int mx, int my, int action, int button) {

	//CalcConstraints();

	float mouseX = engine::ToFloatScreenX(mx);
	float mouseY = engine::ToFloatScreenY(my);

	//bug::outs < "mx " < mouseX < x < (x+w) < "\nmy" < mouseY < y < (y+h) < bug::end;

	// Check if mouse is inside inventory
	if (mouseX > x && mouseX<x + w &&
		mouseY>y && mouseY < y + h) {

		// If there is a container
		if (container != nullptr) {
			float sw = w * 64 / 608.f;
			float sh = h;// *64 / 64.f;

			float gap = w * 4 / 608.f;

			float col = (mouseX - x) / (sw + gap);

			//bug::outs < "col" < col < bug::end;
			//float row = (y+h-mouseY) / sh;

			//bug::outs < row < col < bug::end;

			float rawCol = (col - (int)col) * (sw + gap); // Ignore gap

			if (rawCol < sw && 0 <= col && col <= container->GetSlotWidth()) {

				Item** heldItem = interfaceManager.GetHeldItemPointer();
				Item** item = container->GetItemPointerAt((int)col, 0);

				if (item != nullptr) {
					container->SwitchItem(heldItem, item, button, action);
				}
			}
		}
	}

	return false;
}
Item* inf=nullptr;
void Hotbar::Update(float delta) {

	if (container != nullptr) {
		if (inf == nullptr) {
			inf = new Item("Flint", 20);
		}
		bool temp = container->AddItemAt(inf, 0, 0);
		if (temp) {
			inf = new Item("Fiber", 20);
		}
		else {
			inf->SetItem("Fiber", 20);
		}
		temp = container->AddItemAt(inf, 1, 0);
		if (temp) {
			inf = new Item("Stick", 20);
		}
		else {
			inf->SetItem("Stick", 20);
		}
		temp = container->AddItemAt(inf, 2, 0);
		if (temp) {
			inf = new Item("Flint", 20);
		}
		else {
			inf->SetItem("Flint", 20);
		}
	}

}
void Hotbar::Render() {

	// Inventory background
	engine::BindTexture(0, "containers/hotbar");

	//CalcConstraints();

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
					x + sx * (sw + gap), y + h,
					sw, sh, 1, 1, 1, 1);
			}
		}
	}

}