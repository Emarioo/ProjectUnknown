#include "ProUnk/UI/MasterInventoryPanel.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/UI/InvUtility.h"

namespace prounk {
	bool MasterInventoryPanel::giveItem(Item item) {
		if (m_heldItem.getType() != 0)
			return false;

		m_heldItem = item;
		return true;
	}
	Item MasterInventoryPanel::takeItem() {
		Item item = m_heldItem;
		m_heldItem = {};
		return item;
	}
	void MasterInventoryPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		Renderer* renderer = info.window->getRenderer();
		World* world = m_app->getWorld();

		if (!m_heldItem.getType())
			return;

		ModelId id = m_heldItem.getModelId();
		engone::ModelAsset* asset = world->modelRegistry.getModel(id);

		if (!asset) {
			log::out << log::RED << "MasterInventoryPanel::render - asset is null\n";
			return;
		}

		float mx = GetMouseX();
		float my = GetMouseY();

		float pixelX = mx-pixelSize/2;
		float pixelY = my-pixelSize/2;

		DrawItem(pixelX, pixelY, pixelSize, m_heldItem);
	}
}