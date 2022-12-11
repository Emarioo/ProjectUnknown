#include "ProUnk/UI/InventoryPanel.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/UI/InvUtility.h"

namespace prounk {

	void InventoryPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		Renderer* renderer = info.window->getRenderer();
		World* world = m_app->getWorld();
		EngineObject* object = m_app->playerController.getPlayerObject();

		Inventory* inv = world->inventoryRegistry.getInventory(m_inventoryId);

		if (inv) {
			ui::Box area = getBox();
			area.rgba = { 0.4,0.5,0.6,1 };
			ui::Draw(area);

			// calculate rows and columns for slots
			inv->size();

			int rows = 0;
			int cols = 0;
			float slotSize = 0;

			FontAsset* font = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

			while (true) {
				int index = inv->size() - 1;
				if (index < 1) {
					if (!inv->getItem(index).getType()) {
						inv->addItem({});
					}
					break;
				}
				if (!inv->getItem(index).getType()) {
					if (!inv->getItem(index - 1).getType()) {
						inv->removeItem(index);
					} else {
						break;
					}
				} else {
					inv->addItem({});
					break;
				}
			}

			//ui::TextBox sizes = { std::to_string(area.w) +" "+ std::to_string(area.h),200,20,20,font, {0.,0.2,1,1}};
			//ui::Draw(sizes);
			{
				// equation (;
				// fcols* frows = inv->size();
				// frows = w/h*fcols

				float fcols = sqrt(inv->size() * (area.w / area.h));
				float frows = inv->size()/fcols;

				//ui::TextBox sizes2 = { std::to_string(fcols) + " " + std::to_string(frows),200,40,20,font, {0.,0.2,1,1} };
				//ui::Draw(sizes2);

				cols = ceil(fcols);
				rows = ceil(frows);

				if (rows == 0)
					rows = 1;
				if (cols == 0)
					cols = 1;

				if (rows == 1)
					cols = inv->size();
				else if (cols == 1)
					rows = inv->size();
				
				float maybe1 = area.w / cols;
				float maybe2 = area.h / rows;
				if (maybe1 < maybe2)
					slotSize = maybe1;
				else
					slotSize = maybe2;
			}
			// fill slots
			//log::out << inv->size() << " " << (rows * cols) << "\n";
			//while (inv->size() < cols * rows) {
			//	inv->addItem({});
			//}

			// where to do trimming of items?

			// should also remove slots if necessary

			for (int i = 0; i < inv->size(); i++) {
				Item& item = inv->getItem(i);
				
				float pixelX = m_left;
				float pixelY = m_top;

				int pixelSize = slotSize - 8;

				float slotX = (i % cols) * slotSize;
				float slotY = (i / cols) * slotSize;
				// item/slot position
				pixelX += slotX;
				pixelY += slotY;

				if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)) {
					if (MouseInsideSlot(pixelX, pixelY, slotSize)) {
						//bool yes = m_app->masterInventoryPanel->giveItem(item);
						//if (yes) {
						//	item = {}; // reset item slot
						//	//inv->removeItem(i);
						//}
						if (item.getType()) {
							// pick up item
							bool yes = m_app->masterInventoryPanel->giveItem(item);
							if (yes) {
								// disable stuff for next round
								item.setType(0);
							}
						} else {
							// place item
							Item newItem = m_app->masterInventoryPanel->takeItem();
							if (newItem.getType()) {
								item = newItem;
							}
						}
					}
				}

				DrawSlot(pixelX, pixelY, slotSize);
				if(item.getType())
					DrawItem(pixelX, pixelY, pixelSize, item);
			}
		}
	}
	void InventoryPanel::setInventory(int inventoryId) {
		m_inventoryId = inventoryId;
	}
}