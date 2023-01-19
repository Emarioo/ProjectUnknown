#include "ProUnk/UI/InventoryPanel.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/UI/InvUtility.h"

namespace prounk {

	
	void InventoryPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		CommonRenderer* renderer = GET_COMMON_RENDERER();
		//Renderer* renderer = info.window->getRenderer();
		Session* session = m_app->getActiveSession();
		//EngineObject* object = m_app->playerController.getPlayerObject();

		Inventory* inv = session->inventoryRegistry.getInventory(m_inventoryId);

		ui::Box area = getBox();
		area.rgba = { 0.4,0.5,0.6,1 };
		ui::Draw(area);
		if (inv) {

			// calculate rows and columns for slots
			int cols = 0;
			int rows = 0;
			float slotSize = 0;

			FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

			cols = 4;
			rows = ceil((double)inv->getSlotSize()/cols);

			slotSize = area.w / cols;
			float rowBasedSize = area.h / rows;
			if (rowBasedSize < slotSize)
				slotSize = rowBasedSize;

			//while (true) {
			//	int index = inv->size() - 1;
			//	if (index < 1) {
			//		if (!inv->getItem(index).getType()) {
			//			inv->addItem({});
			//		}
			//		break;
			//	}
			//	if (!inv->getItem(index).getType()) {
			//		if (!inv->getItem(index - 1).getType()) {
			//			inv->removeItem(index);
			//		} else {
			//			break;
			//		}
			//	} else {
			//		inv->addItem({});
			//		break;
			//	}
			//}

			//ui::TextBox sizes = { std::to_string(area.w) +" "+ std::to_string(area.h),200,20,20,font, {0.,0.2,1,1}};
			//ui::Draw(sizes);
			//{
			//	// equation (;
			//	// fcols* frows = inv->size();
			//	// frows = w/h*fcols

			//	float fcols = sqrt(inv->size() * (area.w / area.h));
			//	float frows = inv->size()/fcols;

			//	//ui::TextBox sizes2 = { std::to_string(fcols) + " " + std::to_string(frows),200,40,20,font, {0.,0.2,1,1} };
			//	//ui::Draw(sizes2);

			//	cols = ceil(fcols);
			//	rows = ceil(frows);

			//	if (rows == 0)
			//		rows = 1;
			//	if (cols == 0)
			//		cols = 1;

			//	if (rows == 1)
			//		cols = inv->size();
			//	else if (cols == 1)
			//		rows = inv->size();
			//	
			//	float maybe1 = area.w / cols;
			//	float maybe2 = area.h / rows;
			//	if (maybe1 < maybe2)
			//		slotSize = maybe1;
			//	else
			//		slotSize = maybe2;
			//}
			// fill slots
			//log::out << inv->size() << " " << (rows * cols) << "\n";
			//while (inv->size() < cols * rows) {
			//	inv->addItem({});
			//}

			// where to do trimming of items?

			// should also remove slots if necessary
			int hoveredSlot = -1;
			for (int i = 0; i < inv->getSlotSize(); i++) {
				Item& item = inv->getItem(i);
				
				int pixelSize = slotSize - 8;

				float pixelX = m_left;
				float pixelY = m_top;

				float slotX = (i % cols) * slotSize;
				float slotY = (i / cols) * slotSize;
				// item/slot position
				pixelX += slotX;
				pixelY += slotY;

				if (MouseInsideSlot(pixelX, pixelY, slotSize)) {
					hoveredSlot = i;
					Item& heldItem = m_app->masterInventoryPanel->getItem();
					if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)) {
						if (heldItem.getType() == 0) {
							item.transfer(heldItem);
						} else {
							bool yes = heldItem.transfer(item);
							if (!yes) {
								heldItem.swap(item);
							}
						}
					}
					if (IsKeyPressed(GLFW_MOUSE_BUTTON_2)) {
						if (heldItem.getType() == 0) {
							item.transfer(heldItem, item.getCount() / 2);
						} else {
							int transferAmount = 1;
							if (IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
								transferAmount = 100;
							} else if (IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
								transferAmount = 10;
								//heldItem.transfer(item, ceil(heldItem.getCount() / 2.f)); // ceil here is important
							}
							if (transferAmount > heldItem.getCount())
								transferAmount = heldItem.getCount();
							heldItem.transfer(item, transferAmount);
						}
					}
				}

				DrawSlot(pixelX, pixelY, slotSize);
				if(item.getType())
					DrawItem(pixelX, pixelY, slotSize, item);
			}
			if (hoveredSlot != -1) {
				float pixelX = m_left;
				float pixelY = m_top;

				float slotX = (hoveredSlot % cols) * slotSize;
				float slotY = (hoveredSlot / cols) * slotSize;
				// item/slot position
				pixelX += slotX;
				pixelY += slotY;
				
				Item& item = inv->getItem(hoveredSlot);
				//log::out << item.getType()<<"\n";
				DrawItemToolTip(pixelX + slotSize, pixelY, item);
			}

			const char* keepStr = "KeepInv (active)";
			if (!m_app->playerController.keepInventory) {
				keepStr = "KeepInv (inactive)";
			}
			ui::TextBox keepText = { keepStr,0,0,20,consolas,{1.f} };
			ui::Box keepBox = { 0,0,0,0,{0.f,1.f} };
			keepBox.w = keepText.getWidth();
			keepBox.h = keepText.h;
			keepBox.x = area.x;
			keepBox.y = area.y + area.h - keepBox.h;
			keepText.x = keepBox.x;
			keepText.y = keepBox.y;

			if (ui::Clicked(keepBox) == 1) {
				m_app->playerController.keepInventory = !m_app->playerController.keepInventory;
			}
			ui::Draw(keepBox);
			ui::Draw(keepText);
		}
		DrawToolTip(area.x + area.w, area.y, 20, 20, "This is the inventory");
	}
	void InventoryPanel::setInventory(int inventoryId) {
		m_inventoryId = inventoryId;
	}
}