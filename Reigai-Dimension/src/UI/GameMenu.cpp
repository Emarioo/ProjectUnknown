#include "MenuCreation.h"

namespace UI {
	
	void InitGameMenu() {
		using namespace engine;

		IElement* chatBox = NewElement("chatBox",0);
		{
			chatBox->AddTag([=]() {return IsKey(GLFW_KEY_T); });
			chatBox->Color({ 50 });
			chatBox->Text(GetFont(), "Chatbox", 30, { 0.9f });
			chatBox->conX.Left(20)->conY.Bottom(20)->conW.Center(0.25f)->conH.Center(0.33f);
			//back->NewTransition([=]() {return !GetPauseMode(); })->Move(-200, 0, 1)->Fade({ 50, 0 }, 0.8);
			//back->NewTransition(&back->isHolding)->Fade({ 50 / 3 }, 0.15);
			/*chatBox->OnClick = [](int mx, int my, int button, int action) {
				UIFadeOn();
				AddTimedFunction([=]() {
					SetGameState(Menu);
					UIFadeOff();
				}, 1.f);
			};*/
		}
		/*
		IElement* inventory = NewElement("inventory", 0);
		{
			inventory->AddTag([=]() {return true; });
			inventory->Col({80});
			inventory->conX.Right(20)->conY.Center(0)->conW.Center(0.25f)->conH.Center(0.8f);
		}*/


	}
}