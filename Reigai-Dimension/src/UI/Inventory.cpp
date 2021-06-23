#include "Inventory.h"

#include "Items/ItemHandler.h"
#include "GLFW/glfw3.h"

Inventory::Inventory(const std::string& name) : engine::IBase(name) {
	container = new Container("Player Inv.",5,5);
	container->AddItem(new Item("Rock",23));
	container->AddItem(new Item("BlueRock",320));
	container->AddItem(new Item("Lava",50));
	container->AddItem(new Item("Rock",250));
	container->AddItem(new Item("Lava", 350));
	container->AddItem(new Item("Lava", 230));
	container->AddItem(new Item("Rock", 199));
}
bool Inventory::IsActive() {
	return active;
}
bool Inventory::ClickEvent(int mx, int my, int button, int action) {

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
			float wid = engine::Width();
			float hei = engine::Height();

			float edge = 0.05;
			
			float sw = w * (64/440.f);
			float sh = h* (64 / 850.f);
			
			float startX = w * 63 / 440.f;
			float startY = -h * 241 / 850.f;

			// slots start at pixel (63, 241) slot size 64
			for (int sy = 0; sy < container->GetSlotHeight(); sy++) {
				for (int sx = 0; sx < container->GetSlotWidth(); sx++) {
					Item* item = container->ItemAt(sx, sy);
					 
					if (item != nullptr) {
						// Draw item
						DrawItem(item->GetName(),item->count,
							x-w/2 + startX + sx * sw, y+h/2 + startY + sy * -sh,
							sw, sh);
					}
				}
			}
		}
		
	}
}