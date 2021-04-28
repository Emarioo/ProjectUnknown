#include "Menu.h"

namespace UI {
	bool pause = false;

	void InitPauseMenu() {
		using namespace engine;

		pause = true;// GetPauseMode();

		IElement* back = NewElement("pauseBack", 0);
		{
			back->AddTag([=]() {return IsGameState(Play); });
			back->Col({ 50 });
			back->Text(GetFont(), "Back", 30, { 0.9f });
			back->conX.Left(20)->conY.Center(200)->conW.Center(200)->conH.Center(100);
			back->NewTransition([=]() {return !GetPauseMode(); })->Move(-200, 0, 1)->Fade({ 50, 0 }, 0.8);
			back->NewTransition(&back->isHolding)->Fade({ 50 / 3 }, 0.15);
			back->OnClick = [](int mx, int my, int button, int action) {
				UIFadeOn();
				AddTimedFunction([=]() {
					SetGameState(Menu);
					UIFadeOff();
				}, 1.f);
			};
		}
	}
}