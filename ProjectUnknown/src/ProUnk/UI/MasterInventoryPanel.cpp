#include "ProUnk/UI/MasterInventoryPanel.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/UI/InvUtility.h"

namespace prounk {
	Item& MasterInventoryPanel::getItem() {
		return m_heldItem;
	}
	void MasterInventoryPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		CommonRenderer* renderer = GET_COMMON_RENDERER();
		//Renderer* renderer = info.window->getRenderer();
		Session* session = m_app->getActiveSession();

		if (!m_heldItem.getType())
			return;

		ModelId id = m_heldItem.getModelId();
		engone::ModelAsset* asset = session->modelRegistry.getModel(id);

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