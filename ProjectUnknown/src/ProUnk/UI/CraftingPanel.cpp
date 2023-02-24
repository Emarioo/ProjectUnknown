#include "ProUnk/UI/CraftingPanel.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/Registries/RecipeRegistry.h"

#include "ProUnk/UI/InvUtility.h"

namespace prounk {

	void CraftingPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		CommonRenderer* renderer = GET_COMMON_RENDERER();
		//Renderer* renderer = info.window->getRenderer();
		//Session* session = m_app->getActiveSession();

		//Inventory* inv = world->InventoryRegistry.getInventory(m_inventoryId);

		ui::Box area = getBox();
		area.rgba = { 0.1,0.1,0.5,0.5 };
		ui::Draw(area);

		int clickedIndex = -1;

		for (int i = 0; i < m_craftSlots.size();i++) {
			auto& slot = m_craftSlots[i];
			
			// Todo: Apply some offset to center the whole crafting thing
			float pixelX = area.x + slot.x * slotSize;
			float pixelY = area.y + slot.y * slotSize;

			//log::out << "slot " << pixelX << " " << pixelY << "\n";
			
			if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)) {
				if (MouseInsideSlot(pixelX, pixelY, slotSize)) {
					clickedIndex = i;
				}
			}


			DrawSlot(pixelX, pixelY, slotSize);
			DrawItem(pixelX,pixelY,slotSize,&slot.item);
		}

		if (clickedIndex != -1) {
			Item& heldItem = m_app->masterInventoryPanel->getItem();
			auto& slot = m_craftSlots[clickedIndex];
			if (slot.item.getType()) {
				// pick up item
				bool yes = slot.item.transfer(heldItem);
				if (yes) {
					// disable stuff for next round
					removeNeighbourSlots(slot.x, slot.y);
				}
			} else {
				// place item
				bool yes = heldItem.transfer(slot.item);
				if (yes) {
					addNeighbourSlots(slot.x,slot.y);
				}
			}
		}
		DrawToolTip(area.x + area.w, area.y, 20, 20, "This is Crafting");
	}
	void CraftingPanel::setInventory(int inventoryId) {
		m_inventoryId = inventoryId;
	}
	void CraftingPanel::addNeighbourSlots(int x, int y) {
		std::vector<CraftSlot> newSlots;
		newSlots.push_back({ {},x + 1,y + 1 });
		newSlots.push_back({ {},x + 1,y });
		newSlots.push_back({ {},x + 1,y - 1 });
		newSlots.push_back({ {},x,y + 1 });
		newSlots.push_back({ {},x,y - 1 });
		newSlots.push_back({ {},x - 1,y + 1 });
		newSlots.push_back({ {},x - 1,y });
		newSlots.push_back({ {},x - 1,y - 1 });

		for (int i = 0; i < newSlots.size(); i++) {
			auto& newSlot = newSlots[i];
			bool found = false;
			for (int j = 0; j < m_craftSlots.size(); j++) {
				auto& slot = m_craftSlots[j];
				if (slot.x == newSlot.x && slot.y == newSlot.y) {
					found = true;
					break;
				}
			}
			if (!found) {
				m_craftSlots.push_back(newSlot);
			}
		}
	}
	void CraftingPanel::removeNeighbourSlots(int x, int y) {
		std::vector<bool> safe;
		safe.resize(m_craftSlots.size(), false);
		for (int i = 0; i < m_craftSlots.size(); i++) {
			auto& slot = m_craftSlots[i];
			if (slot.x == 0 && slot.y == 0) {
				safe[i] = true;
			}
			if (slot.item.getType()) {
				safe[i] = true;
			}
			for (int j = 0; j < m_craftSlots.size(); j++) {
				auto& neighbour = m_craftSlots[j];
				if (fabs(slot.x - neighbour.x) <= 1 && fabs(slot.y - neighbour.y) <= 1) {
					if (neighbour.item.getType()) {
						safe[i] = true;
						break;
					}
				}
			}
		}
		for (int i = m_craftSlots.size() - 1; i >= 0; i--) {
			if (!safe[i])
				m_craftSlots.erase(m_craftSlots.begin() + i);
		}
	}
}