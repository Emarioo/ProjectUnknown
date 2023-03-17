#pragma once

#include "ProUnk/UI/PanelHandler.h"

#include "Engone/Assets/ModelAsset.h"

namespace prounk {
	
	class GameApp;

	class InventoryPanel : public Panel {
	public:
		InventoryPanel(GameApp* app) : m_app(app) {}

		void render(engone::LoopInfo& info) override;

		void setInventory(int inventoryId);

	private:
	
		int m_inventoryId=0;

		GameApp* m_app;
	};
}