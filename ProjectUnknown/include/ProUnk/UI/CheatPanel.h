#pragma once

#include "ProUnk/UI/PanelHandler.h"

#include "ProUnk/Registries/InventoryRegistry.h"

#include "Engone/Structures/Array.h"

namespace prounk {

	class GameApp;

	class CheatPanel : public Panel {
	public:
		CheatPanel(GameApp* app);

		void render(engone::LoopInfo& info) override;

	private:

		//std::vector<Item> availableItems;
		engone::Array availableItems{sizeof(Item),ALLOC_TYPE_HEAP};

		float maxAnimTime = 0.24f; // seconds
		float animTime = 0;
		int itemSlot = -1; // slot in availableItems

		GameApp* m_app;
	};
}