
#include "ProUnk/UI/UIMenus.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	void IntroScreen(GameApp* app) {
		using namespace engone;
		//float time = GetAppTime();

		const float black = 2, fade = black + .5, intro = fade + 2, introOut = intro + .5, gameIn = introOut + .5;

		//if (time < black) { // black 2s
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 0.f,0.f,0.f });
		//} else if (time < fade) { // fade 0.5s
		//	float alpha = 1 - (time - black) / .5f;
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 1.f, 1.f, 1.f });
		//	ui::Draw({ GetAsset<Texture>("textures/intro"), 0.f, 0.f, GetWidth(), GetHeight() });
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 0.f,0.f,0.f, alpha });
		//} else if (time < intro) { // intro 2s
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 1.f, 1.f, 1.f });
		//	ui::Draw({ GetAsset<Texture>("textures/intro"), 0.f, 0.f, GetWidth(), GetHeight() });
		//} else if (time < introOut) { // fade 0.5s
		//	float alpha = (time - intro) / .5f;
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 1.f, 1.f, 1.f });
		//	ui::Draw({ GetAsset<Texture>("textures/intro"), 0.f, 0.f, GetWidth(), GetHeight() });
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 0.f,0.f,0.f, alpha });
		//} else if (time < gameIn) { // fade 0.5s
		//	float alpha = 1 - (time - introOut) / .5f;
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 0.f,0.f,0.f, alpha });
		//}

		/*IElement* cover = new IElement("darkCover", 9999);
		cover->CenterX(0)->CenterY(0)->Width(1.f)->Height(1.f)->Color({ 0.f });
		cover->add("fade").Fade({ 0.f,0.f }, .5f);

		IElement* intro = new IElement("introTexture");
		intro->Left(0)->CenterY(0)->Width(1.f)->Height(1.f)->Fixed(800 / 669.f)->Color({ 1.f })->Texture(GetAsset<Texture>("textures/intro"));
		IElement* blank = new IElement("introBlank", 9990);
		blank->CenterX(0)->CenterY(0)->Width(1.f)->Height(1.f)->Color({ 1.f });
		blank->add(intro);

		AddTimer(2.f, [=] {
			cover->setTransition("fade", true);
			AddTimer(.5 + 2.f, [=] {
				cover->setTransition("fade", false);
				AddTimer(.5f, [=] {
					RemoveElement(blank);
					cover->setTransition("fade", true);
					SetState(GameState::RenderGame, true);
					SetState(GameState::Intro, false);
					GetWindow()->lockCursor(true);
					});
				});
			});

		AddElement(blank);
		AddElement(cover);*/
	}
}