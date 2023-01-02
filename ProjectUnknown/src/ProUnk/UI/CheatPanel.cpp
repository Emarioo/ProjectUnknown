#include "ProUnk/UI/CheatPanel.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/UI/InvUtility.h"

namespace prounk {
	CheatPanel::CheatPanel(GameApp* app) : m_app(app) {
		Session* sess = app->getActiveSession();
		const ItemTypeInfo* info = sess->itemTypeRegistry.getType("sword");
		availableItems.push_back(Item(info->itemType,1));
		ComplexData* data = sess->complexDataRegistry.registerData();
		ComplexPropertyType* atk = sess->complexDataRegistry.getProperty("atk");
		ComplexPropertyType* knock = sess->complexDataRegistry.getProperty("knock");
		data->set(atk,30.f);
		data->set(knock,0.3f);
		availableItems.back().setComplexData(data->getDataIndex());
		
		//const ItemTypeInfo* info = sess->itemTypeRegistry.getType("sword");
		availableItems.push_back(Item(info->itemType, 1));
		//ComplexData* 
			data = sess->complexDataRegistry.registerData();
		//ComplexPropertyType* atk = sess->complexDataRegistry.getProperty("atk");
		//ComplexPropertyType* knock = sess->complexDataRegistry.getProperty("knock");
		data->set(atk, 40.f);
		data->set(knock, 5.3f);
		availableItems.back().setComplexData(data->getDataIndex());

	}
	void CheatPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		Renderer* renderer = info.window->getRenderer();
		Session* session = m_app->getActiveSession();
		EngineObject* object = m_app->playerController.getPlayerObject();
		
		Inventory* inv = m_app->playerController.getInventory();

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
			Item& item = availableItems[i];
			if (item.getType() == 0) {
				availableItems.erase(availableItems.begin() + i);
				i--;
				continue;
			}

			if (IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
				item.setCount(100);
			} else if (IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
				item.setCount(10);
			} else {
				item.setCount(1);
			}

			float pixelX = area.x+i * itemSize;
			float pixelY = area.y;

			ui::Box outer = { pixelX,pixelY,itemSize,itemSize,{0.2f,0.8f} };

			DrawSlot(pixelX, pixelY, itemSize);
			DrawItem(pixelX, pixelY, itemSize, item);

			if (MouseInsideSlot(pixelX, pixelY, itemSize)) {
				hoveredItem = i;


				if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)) {

					uint32_t slot = inv->findAvailableSlot(item);
					bool yes = item.copy(inv->getItem(slot));
					if (yes) {
						itemSlot = i;
						animTime = maxAnimTime;
					}
				}
			}
		}

		if (hoveredItem != -1) {
			float pixelX = area.x + hoveredItem*itemSize;
			float pixelY = area.y;

			Item& item = availableItems[hoveredItem];
			if (IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
				item.setCount(100);
			} else if (IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
				item.setCount(10);
			} else {
				item.setCount(1);
			}
			DrawItemToolTip(pixelX + itemSize, pixelY, item);
		}

		setMinHeight(itemSize);
		setMinWidth(availableItems.size() * itemSize);
	}
}