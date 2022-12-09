#include "ProUnk/UI/InventoryPanel.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	void InventoryPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		Renderer* renderer = info.window->getRenderer();
		World* world = m_app->getWorld();
		EngineObject* object = m_app->playerController.getPlayerObject();
		//Inventory* inv = world->getInventory(object);

		Inventory* inv = world->InventoryRegistry.getInventory(m_inventoryId);

		//if (IsScrolledY()) {
		//	itemSize *= 1 + IsScrolledY()*0.1;
		//}
		//log::out << "scale " << itemScale << " "<<GetWidth() <<" "<< GetHeight() << "\n";
		
		if (inv) {
			ui::Box area = getBox();
			area.rgba = { 0.4,0.5,0.6,1 };
			ui::Draw(area);



			// calculate rows and columns for slots
			inv->size();

			int rows = 0;
			int cols = 0;
			float slotSize = 0;

			FontAsset* font = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

			//ui::TextBox sizes = { std::to_string(area.w) +" "+ std::to_string(area.h),200,20,20,font, {0.,0.2,1,1}};
			//ui::Draw(sizes);
			{
				// equation (;
				// fcols* frows = inv->size();
				// frows = w/h*fcols

				float fcols = sqrt(inv->size() * (area.w / area.h));
				float frows = inv->size()/fcols;

				//ui::TextBox sizes2 = { std::to_string(fcols) + " " + std::to_string(frows),200,40,20,font, {0.,0.2,1,1} };
				//ui::Draw(sizes2);

				cols = round(fcols);
				rows = round(frows);

				if (rows == 0)
					rows = 1;
				if (cols == 0)
					cols = 1;

				if (rows == 1)
					cols = inv->size();
				else if (cols == 1) 
					rows = inv->size();
				
				float maybe1 = area.w / cols;
				float maybe2 = area.h / rows;
				if (maybe1 < maybe2)
					slotSize = maybe1;
				else
					slotSize = maybe2;
			}

			for (int i = 0; i < inv->size(); i++) {
				Item& item = inv->getItem(i);
				ModelId id = item.getModelId();
				engone::ModelAsset* asset = world->ModelRegistry.getModel(id);
				if (asset) {
					float pixelX = m_left;
					float pixelY = m_top;

					int pixelSize = slotSize - 8;

					float slotX = (i % cols) * slotSize;
					float slotY = (i / cols) * slotSize;
					// item/slot position
					pixelX += slotX;
					pixelY += slotY;

					// model offset
					float modelX = -asset->boundingPoint.x; // where to render model in model space
					float modelY = -asset->boundingPoint.y;

					glm::vec3 modelScale = { 1,1,1 };
					modelScale.x = 2.f * pixelSize / GetWidth();
					modelScale.y = 2.f * pixelSize / GetHeight();

					float scaleW = asset->maxPoint.x - asset->minPoint.x;
					float scaleH = asset->maxPoint.y - asset->minPoint.y;
					if (scaleW > scaleH) {
						modelScale /= scaleW;
					} else {
						modelScale /= scaleH;
					}

					// pixel to model
					modelX += (pixelX+ slotSize /2-GetWidth() / 2) * 2.f / GetWidth() / modelScale.x;
					modelY += (GetHeight()/2- slotSize /2-pixelY) * 2.f / GetHeight() / modelScale.y;

					glm::mat4 matrix = glm::translate(glm::vec3(0, 0, -1.0))  * glm::scale(modelScale) * glm::translate(glm::vec3(modelX, modelY, 0));
					//renderer->DrawOrthoModel(asset, matrix);
					ui::Draw(asset, matrix);
					{
						ui::Box box = { pixelX,pixelY,slotSize,slotSize,{0.0,0.0,0.0,0.1} };
						ui::Draw(box);
					}
					//{
					//	ui::Box box = { pixelX,pixelY,pixelSize,pixelSize,{0.9,0.9,0.9,1} };
					//	//box.x -= box.w / 2;
					//	//box.y -= box.h / 2;
					//	ui::Draw(box);
					//}
				} else {
					log::out << log::RED << "RenderInv - asset is null\n";
				}
			}
		}
	}
	void InventoryPanel::setInventory(int inventoryId) {
		m_inventoryId = inventoryId;
	}
}