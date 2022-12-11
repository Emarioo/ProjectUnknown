#include "ProUnk/UI/InvUtility.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	void DrawItem(float x, float y, float size, Item item) {
		using namespace engone;

		if (!item.getType())
			return;

		auto win = engone::GetActiveWindow();
		World* world = (World*)win->getParent()->getWorld();

		if (!world) {
			log::out << log::RED<<"InvUtility DrawItem missing world\n";
			return;
		}
		ModelId id = item.getModelId();
		engone::ModelAsset* asset = world->modelRegistry.getModel(id);
		if (!asset) {
			return;
		}
		float pixelX = x;
		float pixelY = y;

		// model offset
		float modelX = -asset->boundingPoint.x; // where to render model in model space
		float modelY = -asset->boundingPoint.y;

		glm::vec3 modelScale = { 1,1,1 };
		modelScale.x = 2.f * size / GetWidth();
		modelScale.y = 2.f * size / GetHeight();

		float scaleW = asset->maxPoint.x - asset->minPoint.x;
		float scaleH = asset->maxPoint.y - asset->minPoint.y;
		if (scaleW > scaleH) {
			modelScale /= scaleW;
		} else {
			modelScale /= scaleH;
		}

		// pixel to model
		modelX += (pixelX + size / 2 - GetWidth() / 2) * 2.f / GetWidth() / modelScale.x;
		modelY += (GetHeight() / 2 - size / 2 - pixelY) * 2.f / GetHeight() / modelScale.y;

		glm::mat4 matrix = glm::translate(glm::vec3(0, 0, -1.0)) * glm::scale(modelScale) * glm::translate(glm::vec3(modelX, modelY, 0));
		ui::Draw(asset, matrix);
	}
	void DrawSlot(float x, float y, float size) {
		using namespace engone;

		float pixelX = x;
		float pixelY = y;

		ui::Box box = { pixelX,pixelY,size,size,{0.6,0.7,0.8,0.5} };
		//ui::Box box = { pixelX,pixelY,size,size,{0.0,0.0,0.0,0.1} };
		ui::Draw(box);
	}
	bool MouseInsideSlot(float slotX, float slotY, float slotSize) {
		using namespace engone;
		float mx = GetMouseX();
		float my = GetMouseY();
		return (slotX <= mx && slotX + slotSize >= mx && slotY <= my && slotY + slotSize >= my);
	}
}