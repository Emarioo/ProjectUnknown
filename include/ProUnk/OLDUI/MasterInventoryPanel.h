#pragma once

#include "Engone/Assets/ModelAsset.h"

#include "ProUnk/UI/PanelHandler.h"

#include "ProUnk/Registries/InventoryRegistry.h"

namespace prounk {

	class GameApp;

	class MasterInventoryPanel : public Panel {
	public:
		MasterInventoryPanel(GameApp* app) : m_app(app) {}

		void render(engone::LoopInfo& info) override;

		Item& getItem();

	private:

		float pixelSize = 45;

		Item m_heldItem;
		GameApp* m_app;
	};
}