#pragma once

#include "ProUnk/UI/PanelHandler.h"

#include "Engone/Assets/ModelAsset.h"

#include "ProUnk/DataRegistries/InventoryRegistry.h"

namespace prounk {

	class GameApp;

	struct CraftSlot {
		Item item;
		int x, y;
	};

	class CraftingPanel : public Panel {
	public:
		CraftingPanel(GameApp* app) : m_app(app) {
			m_craftSlots.push_back({{},0,0});
		}

		void render(engone::LoopInfo& info) override;

		// inventory where items will be taken from
		// can change this later if you want to items from a chest panel to be included
		void setInventory(int inventoryId);

		void addNeighbourSlots(int x, int y);
		void removeNeighbourSlots(int x, int y);

	private:

		float slotSize=35;

		int m_inventoryId = 0;

		std::vector<CraftSlot> m_craftSlots;

		GameApp* m_app;
	};
}