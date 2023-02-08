#include "ProUnk/UI/InvUtility.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	void DrawItem(float x, float y, float size, Item item) {
		using namespace engone;

		if (!item.getType())
			return;

		auto win = engone::GetActiveWindow();
		Session* session = ((GameApp*)win->getParent())->getActiveSession();

		if (!session) {
			log::out << log::RED<<"InvUtility DrawItem missing session\n";
			return;
		}
		ModelId id = item.getModelId();
		engone::ModelAsset* asset = session->modelRegistry.getModel(id);
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

		//Window* win = engone::GetActiveWindow();
		FontAsset* consolas = win->getStorage()->get<FontAsset>("fonts/consolas42");

		const ui::Color counterColor = { 1.f };
		//ui::TextBox counter = {"|",0,0,size*0.5,consolas,counterColor};
		ui::TextBox counter = {std::to_string(item.getCount()),0,0,size*0.5f,consolas,counterColor};
		float wid = counter.getWidth();
		float some = 1.1f;
		counter.x = pixelX + size*some-wid*some;
		counter.y = pixelY + size*some - counter.h/some;
		ui::Draw(counter);
	}
	void DrawSlot(float x, float y, float size) {
		using namespace engone;

		// Color
		const ui::Color borderColor = { 0.5,0.5,0.5,0.5 };
		const ui::Color innerColor = { 0.6,0.7,0.8,0.5 };

		float pixelX = x;
		float pixelY = y;

		float padding = 2;
		ui::Box borderBox = { pixelX,pixelY,size,size,borderColor };
		ui::Box innerBox = { pixelX+padding,pixelY+padding,size-padding*2,size-padding*2,innerColor };
		ui::Draw(borderBox);
		ui::Draw(innerBox);
	}
	bool MouseInsideSlot(float slotX, float slotY, float slotSize) {
		using namespace engone;
		float mx = GetMouseX();
		float my = GetMouseY();
		return (slotX <= mx && slotX + slotSize >= mx && slotY <= my && slotY + slotSize >= my);
	}
	void DrawToolTip(float x, float y, float size, float textHeight, const std::string& description) {
		using namespace engone;
		Window* win = engone::GetActiveWindow();
		FontAsset* consolas = win->getStorage()->get<FontAsset>("fonts/consolas42");

		//-- Colors (these should be tweaked)
		const ui::Color littleBoxColor = { 0.23f,0.23f,0.28f,1.f };
		const ui::Color littleTextColor = { 1.f };
		const ui::Color descBoxColor = { 0.2f,0.8f };
		const ui::Color descTextColor = { 0.75f,0.97,0.97,.98f };

		//-- Little ? box
		ui::Box littleBox = { x,y,size,size,littleBoxColor };

		ui::TextBox littleText = { "?",0,0,littleBox.h,consolas,littleTextColor };
		littleText.x = littleBox.x+littleBox.w/2-littleText.getWidth()/2;
		littleText.y = littleBox.y+littleBox.h/2-littleText.h/2;

		ui::Draw(littleBox);
		ui::Draw(littleText);

		// could add some smooth animation and transition for description

		//-- Actual Tooltip
		if (ui::Hover(littleBox)) {

			ui::TextBox descText = { description,0,0,textHeight,consolas, descTextColor };
			float descWidth = descText.getWidth();
			float descHeight = descText.getHeight();

			ui::Box descBox = { littleBox.x+littleBox.w,littleBox.y,descWidth,descHeight,descBoxColor };
			
			float spaceRight = win->getWidth() - (littleBox.x + littleBox.w);
			float spaceLeft = littleBox.x;
			float spaceDown = win->getHeight() - (littleBox.y + littleBox.h);
			float spaceUp = littleBox.y;
			
			//-- Description positioning
			if (spaceRight >= descWidth) {
				// Right side of little box
				descBox.x = littleBox.x + littleBox.w;
				if (win->getHeight()-littleBox.y<descHeight) {
					descBox.y = win->getHeight() - descHeight;
				} else {
					descBox.y = littleBox.y;
				}
			} else if (spaceUp >= descHeight) {
				// Above little box
				descBox.x = win->getWidth() - descWidth;
				descBox.y = littleBox.y - descHeight;
			}else if (spaceDown >= descHeight) {
				// Below little box
				descBox.x = win->getWidth() - descWidth;
				descBox.y = littleBox.y + littleBox.h;
			} else if (spaceLeft >= descWidth) {
				// Left side of little box
				descBox.x = littleBox.x - descWidth;
				descBox.y = littleBox.y;
			} else {
				// Wherever
				descBox.x = littleBox.x + littleBox.w / 2 - descWidth / 2;
				descBox.y = littleBox.y+littleBox.h/2-descHeight/2;
			}
		
			descText.x = descBox.x;
			descText.y = descBox.y;

			ui::Draw(descBox);
			ui::Draw(descText);
		}
	}
	void DrawItemToolTip(float x, float y, float itemSize, Item& item) {
		using namespace engone;

		if (item.getType() == 0)
			return;

		Window* win = engone::GetActiveWindow();
		FontAsset* consolas = win->getStorage()->get<FontAsset>("fonts/consolas42");

		//-- Colors (these should be tweaked)
		const ui::Color areaColor = { 0.1,0.1,0.15f,0.62f };
		const ui::Color standardColor = { 1.f };
		const ui::Color propColor = { 1.f,0.95f,0.95,1.f };
		const ui::Color hiddenColor = { 0.7f,1.f };

		float padding = 3;

		GameApp* app = (GameApp*)win->getParent();
		Session* session = app->getActiveSession();

		const ItemTypeInfo* typeInfo = session->itemTypeRegistry.getType(item.getType());
		if (!typeInfo)
			return;

		char str[50];
		snprintf(str, sizeof(str), "%s (%dx)", item.getDisplayName().c_str(), item.getCount());
		
		ui::TextBox standardInfo = { str,0,0,20,consolas,standardColor };

		// Todo: use a vector for all the texts. not only propTexts. I think the code can be compacted.

		std::vector<ui::TextBox> propTexts;
		float propHeight = 0;
		
		auto complexData = item.getComplexData();
		//auto complexData = session->complexDataRegistry.getData(item.getComplexData());
		if (complexData) {
			auto& list = complexData->getList();
			for (int i = 0; i < list.size();i++) {
				uint32_t key = list[i].prop;
				float value = list[i].value;
				ComplexPropertyType* prop = session->complexDataRegistry.getProperty(key);
				if (!prop) {
					log::out << log::RED << "DrawItemTooltip : " << key << " is not a valid index for property";
					continue;
				}
				// assume float
				snprintf(str, sizeof(str), "%s : %.2f", prop->name.c_str(), value);

				ui::TextBox propText = { str,0,0,20,consolas,propColor };
				propTexts.push_back(propText);
				propHeight += propText.h;
			}
			//auto& map = complexData->getMap();
			//HashMap::IterationInfo info;
			//while (map.iterate(info)) {
			//	
			//	ComplexPropertyType* prop = session->complexDataRegistry.getProperty(info.key);
			//	if (!prop) {
			//		log::out << log::RED << "DrawItemTooltip : "<<info.key<<" is not a valid index for property";
			//		continue;
			//	}
			//	float value = *(float*)&info.value;
			//	// assume float
			//	snprintf(str, sizeof(str), "%s : %.2f", prop->name.c_str(), value);

			//	ui::TextBox propText = { str,0,0,20,consolas,propColor };
			//	propTexts.push_back(propText);
			//	propHeight += propText.h;
			//}
		}

		snprintf(str, sizeof(str), "%s", typeInfo->name.c_str());
		ui::TextBox hiddenInfo = { str,0,0,20,consolas,hiddenColor };

		ui::Box area = { 0,0,0,0,areaColor };
		area.x = x;
		area.y = y;
		area.w = 0;
		area.h = hiddenInfo.h + propHeight +standardInfo.h + 6;
		float stoof[] = { standardInfo.getWidth() , hiddenInfo.getWidth() };
		for (int i = 0; i < sizeof(stoof) / sizeof(float); i++) {
			if (stoof[i] > area.w) {
				area.w = stoof[i];
			}
		}
		for (ui::TextBox& textBox : propTexts) {
			float width = textBox.getWidth();
			if (width > area.w) {
				area.w = width;
			}
		}
		area.w += 6;

		float spaceRight = win->getWidth() - (x + itemSize);
		float spaceLeft = x;
		float spaceDown = win->getHeight() - (y + itemSize);
		float spaceUp = y;

		//-- Description positioning
		if (spaceRight >= area.w) {
			// Right side of little box
			area.x = x + itemSize;
			if (win->getHeight() < y+area.h) {
				area.y = win->getHeight() - area.h;
			} else {
				area.y = y;
			}
		} else if (spaceUp >= area.h) {
			// Above little box
			area.x = win->getWidth() - area.w;
			area.y = y - area.h;
		} else if (spaceDown >= area.h) {
			// Below little box
			area.x = win->getWidth() - area.w;
			area.y = y + itemSize;
		} else if (spaceLeft >= area.w) {
			// Left side of little box
			area.x = x - area.w;
			area.y = y;
		} else {
			// Wherever
			area.x = x + itemSize / 2 - area.w / 2;
			area.y = y + itemSize / 2 - area.h / 2;
		}

		//descText.x = descBox.x;
		//descText.y = descBox.y;

		float edge=area.y+padding;

		standardInfo.x = area.x + area.w/2-standardInfo.getWidth()/2;
		standardInfo.y = edge;
		
		edge += standardInfo.h;

		for (ui::TextBox& textBox : propTexts) {
			textBox.x = area.x + area.w / 2 - textBox.getWidth() / 2;
			textBox.y = edge;
			edge += textBox.h;
		}
		
		hiddenInfo.x = area.x + area.w/2-hiddenInfo.getWidth()/2;
		hiddenInfo.y = edge;
		edge += hiddenInfo.h;
		
		ui::Draw(area);
		ui::Draw(standardInfo);
		for (ui::TextBox& textBox : propTexts) {
			ui::Draw(textBox);
		}
		ui::Draw(hiddenInfo);
	}
}