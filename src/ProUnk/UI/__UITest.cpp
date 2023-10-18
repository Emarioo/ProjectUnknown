
#include "ProUnk/UI/UIMenus.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	//engone::ui::TextBox editText = { "012345",50,100,50 };
		//std::string hey = "Okay then";
		//int indexing = hey.length();
	void UiTest(GameApp* app) {
		using namespace engone;
		//editText.y += 0.05;

		/*ui::Edit(hey, indexing);
		ui::TextBox wow = { hey, 50, 100, 30 };
		wow.font = GetAsset<Font>("consolas");
		wow.at = indexing;
		wow.edited = true;
		ui::Draw(wow);*/

		/*if (editText.font == nullptr) {
			editText.font = GetAsset<Font>("consolas");
		}*/
		//ui::Draw({ "Hejsan", 100, 100, 30, 0.5f, 0.5f, 0.5f});
		/*ui::Draw({50,100,100,100,1,0,0});
		ui::Draw({GetAsset<Texture>("textures/wall"), 500, 100, 400, 400});*/

		//ui::Box fullscreen = {100+2,100,100,100,1.f,1.f,0};
		//ui::Box windowed = {100,400,100,100,1.f,0,0};
		//ui::Box borderless = {100,600,100,100,1.f,0,1.f};

		//float w = editText.font->getWidth(editText.text, editText.h);
		//ui::Draw({editText.x,editText.y,w,editText.h,1.f,0,0});

		//ui::Draw(fullscreen);
		///*ui::Draw(windowed);
		//ui::Draw(borderless);*/
		//if (ui::Hover(editText)) {
		//	ui::Edit(&editText);
		//}

		//ui::Draw(editText);

		/*if (ui::Clicked(fullscreen)) {
			GetWindow()->setMode(WindowMode::Fullscreen);
		}
		if (ui::Clicked(windowed)) {
			GetWindow()->setMode(WindowMode::Windowed);
		}
		if (ui::Clicked(borderless)) {
			GetWindow()->setMode(WindowMode::BorderlessFullscreen);
		}*/
	}
}