#include "ProUnk/UI/CheatPanel.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/UI/InvUtility.h"

namespace prounk {
	CheatPanel::CheatPanel(GameApp* app) : m_app(app) {
		// Session* sess = app->getActiveSession();
		// const ItemTypeInfo* sword = sess->itemTypeRegistry.getType("sword");
		// const ItemTypeInfo* dagger = sess->itemTypeRegistry.getType("dagger");
		// const ItemTypeInfo* spear = sess->itemTypeRegistry.getType("spear");

		// ComplexPropertyType* atk = sess->complexDataRegistry.getProperty("atk");
		// ComplexPropertyType* knock = sess->complexDataRegistry.getProperty("knock");

		// Item tmp = { sword,1 };
		// Item* item = 0;
		// availableItems.add(&tmp,(void**)&item);
		// //availableItems.push_back(Item(sword, 1));
		// //ComplexData* data = availableItems.back().getComplexData();
		// item->getComplexData()->set(atk,25.f);
		// item->getComplexData()->set(knock,0.2f);

		// Item* ditem = 0;
		// tmp = { dagger,1 };
		// availableItems.add(&tmp, (void**)&ditem);
		// //availableItems.push_back(Item(dagger, 1));
		// //data = availableItems.back().getComplexData();
		// item->getComplexData()->set(atk, 15);
		// item->getComplexData()->set(knock, 0.1f);

		// Item* sitem = 0;
		// tmp = { spear,1 };
		// availableItems.add(&tmp, (void**)&sitem);

		// Item* ehm = (Item*)availableItems.get(0);

		// //availableItems.push_back(Item(spear, 1));
		// //data = availableItems.back().getComplexData();
		// item->getComplexData()->set(atk, 40.f);
		// item->getComplexData()->set(knock, 0.3f);
	}
	void CheatPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		CommonRenderer* renderer = info.window->getCommonRenderer();
		//Renderer* renderer = info.window->getRenderer();
		// Session* session = m_app->getActiveSession();

		// //EngineObject* object = m_app->playerController.getPlayerObject();
		
		// Inventory* inv = m_app->playerController.getInventory();

		ui::Box area = getBox();
		area.rgba = { 0.4,0.5,0.6,1 };
		ui::Draw(area);

		float itemSize = 45;
		
		int hoveredItem=-1;
		
		if (itemSlot != -1) {
			animTime -= info.timeStep;
			if (animTime <= 0) {
				itemSlot = 0;
				animTime = 0;
			} else {
				float pixelX = area.x + itemSlot * itemSize;
				float pixelY = area.y;
				float scale = animTime / maxAnimTime;
				const ui::Color color = { 1.f,1.0f,1.0f,1.f };
				ui::Box box = { pixelX + itemSize*(1- scale)/2,pixelY+itemSize*(1-scale)/2,itemSize * scale,itemSize * scale,color};
				ui::Draw(box);
			}
		}

		for (int i = 0; i < availableItems.size(); i++) {
			Item* item = (Item*)availableItems.get(i);
			//if (item) {
			//	log::out << "item "<<item->getDisplayName()<<"\n";
			//}
			if (item->getType() == 0) {
				item->~Item();
				availableItems.remove(i);
				//availableItems.erase(availableItems.begin() + i);
				i--;
				continue;
			}

			if (IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
				item->setCount(100);
			} else if (IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
				item->setCount(10);
			} else {
				item->setCount(1);
			}

			float pixelX = area.x+i * itemSize;
			float pixelY = area.y;

			ui::Box outer = { pixelX,pixelY,itemSize,itemSize,{0.2f,0.8f} };

			DrawSlot(pixelX, pixelY, itemSize);
			DrawItem(pixelX, pixelY, itemSize, item);

			if (MouseInsideSlot(pixelX, pixelY, itemSize)) {
				hoveredItem = i;

				if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)) {
					// uint32_t slot = inv->findAvailableSlot(item);
					// if (slot != -1) {
					// 	bool yes = item->copy(*inv->getItem(slot));
					// 	if (yes) {
					// 		itemSlot = i;
					// 		animTime = maxAnimTime;
					// 	}
					// }
				}
			}
		}

		if (hoveredItem != -1) {
			float pixelX = area.x + hoveredItem*itemSize;
			float pixelY = area.y;

			Item* item = (Item*)availableItems.get(hoveredItem);
			if (IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
				item->setCount(100);
			} else if (IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
				item->setCount(10);
			} else {
				item->setCount(1);
			}
			DrawItemToolTip(pixelX, pixelY, itemSize, item);
		}

		setMinHeight(itemSize);
		setMinWidth(availableItems.size() * itemSize);

		DrawToolTip(area.x + area.w, area.y, 20, 20, "Cheat panel here.\nClick to give yourself items.");
	}
}