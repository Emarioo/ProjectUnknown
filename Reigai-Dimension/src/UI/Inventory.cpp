#include "Inventory.h"

#include "Items/ItemHandler.h"

Inventory::Inventory(const std::string& name) : ISystem(name) {

}
void Inventory::MouseEvent(double mx, double my, int button, int action) {

}
void Inventory::Update(float delta) {

}
void Inventory::Render() {
	engine::DrawRect(0.5,0.5,0.5,0.5,0.1,0.1,0.1,0.1);
	if (container != nullptr) {
		for (int y = 0; y < container->GetSlotHeight(); y++) {
			for (int x = 0; x < container->GetSlotWidth(); x++) {
				Item* item = container->ItemAt(x,y);
				if (item!=nullptr) {
					// Draw item
					DrawItem(item->GetName(),x*0.1,y*0.1,0.1,0.1);
				}
			}
		}
	}

}