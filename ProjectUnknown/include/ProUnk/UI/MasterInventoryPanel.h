#pragma once

#include "Engone/Assets/ModelAsset.h"

#include "ProUnk/UI/PanelHandler.h"

#include "ProUnk/DataRegistries/InventoryRegistry.h"

namespace prounk {

	class GameApp;

	class MasterInventoryPanel : public Panel {
	public:
		MasterInventoryPanel(GameApp* app) : m_app(app) {}

		void render(engone::LoopInfo& info) override;

		// returns false if there already is a held item
		bool giveItem(Item item);
		// returns item with type 0 if there is no held item
		Item takeItem();

	private:

		int pixelSize = 35;

		Item m_heldItem;
		GameApp* m_app;
	};
}