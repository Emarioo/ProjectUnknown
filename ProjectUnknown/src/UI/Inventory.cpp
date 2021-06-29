#include "Inventory.h"

#include "Items/ItemHandler.h"
#include "GLFW/glfw3.h"
#include "InterfaceManager.h"

Inventory::Inventory(const std::string& name) : engine::IBase(name) {
	container = new Container("Player Inv.",5,5);
	container->AddItem(new Item(FlintItem,23));
	container->AddItem(new Item(StickItem,320));
	container->AddItem(new Item(FiberItem,50));
	container->AddItem(new Item(StickItem,250));
	container->AddItem(new Item(FiberItem, 111));
}
bool Inventory::IsActive() {
	return active;
}
bool Inventory::ClickEvent(int mx, int my, int button, int action) {
	CalcConstraints();

	float mouseX = engine::ToFloatScreenX(mx);
	float mouseY = engine::ToFloatScreenY(my);

	// Check if mouse is inside inventory
	if (mouseX>x-w/2&&mouseX<x+w/2&&
		mouseY>y-h/2&&mouseY<y+h/2) {
		
		// If there is a container
		if (container!=nullptr) {
			float sw = w * (64 / 440.f);
			float sh = h * (64 / 850.f);

			float startX = w * 63 / 440.f;
			float startY = -h * 241 / 850.f;

			float row = (mouseX-startX-x+w/2)/sw;
			float col = (mouseY-startY-y-h/2)/-sh;

			//bug::outs < row < col < bug::end;

			if (row > 0 && col > 0&&row<container->GetSlotWidth()&&col<container->GetSlotHeight()) {
				
				Item** heldItem = UI::GetHeldItemPointer();
				Item** item = container->GetItemPointerAt((int)row, (int)col);
				
				if (item != nullptr) {
					container->SwitchItem(heldItem, item,button,action);
				}
			}
		}
	}

	return false;
}
bool Inventory::KeyEvent(int key, int action) {
	if (key==GLFW_KEY_E) {
		if (action == 1) {
			active = !active;
			engine::SetCursorMode(active);
		}
	}
	return false;
}
void Inventory::Update(float delta) {
	if (IsActive()) {
		
	}
}
void Inventory::Render() {
	//bug::out < "Invetory Draw" < bug::end;
	if (IsActive()) {
		// Inventory background
		engine::BindTexture(0, "containers/inventory");
		
		CalcConstraints();

		engine::DrawRect(x, y, w, h, color.r, color.g, color.b, color.a);
		
		// Items
		if (container != nullptr) {
			float sw = w * (64/440.f);
			float sh = h* (64 / 850.f);
			
			float startX = w * 63 / 440.f;
			float startY = -h * 241 / 850.f;

			// slots start at pixel (63, 241) slot size 64
			for (int sy = 0; sy < container->GetSlotHeight(); sy++) {
				for (int sx = 0; sx < container->GetSlotWidth(); sx++) {
					Item* item = container->GetItemAt(sx, sy);
					 
					if (item != nullptr) {
						// Draw item
						DrawItem(item,
							x-w/2 + startX + sx * sw, y+h/2 + startY + sy * -sh,
							sw, sh);
					}
				}
			}
		}
		
	}
}