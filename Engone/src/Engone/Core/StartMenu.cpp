#include "StartMenu.h"

namespace startMenu {

	bool menu = true;
	bool hideStart = false;
	bool showAlone = false;
	bool editWorld = false;
	int editId = 0;
	void Init() {
		//using namespace iManager;

		//menu = dManager::GetGameState()==GameState::menu;
		/*
		IElement* red = NewElement("red");
		red->Color(1.f, 0.f, 0.f, 1)->Text(GetFont(),"This is\n a lot of\n experimental text\n as you can see",30,1,1,1,1)->
			conX.Center(0)->conY.Center(0)->conW.Center(0.25f)->conH.Center(100);
		red->NewTransition(&hideAlone)->Fade(1.f, 0.f,0.f, 0.f,2.f);
		*/
		/*
		IElement* alone = NewElement("playAlone",5);
		alone->AddTag(&menu);
		alone->Color(63, 63, 107, 1)->Text(GetFont(),"Play Alone",30,0.9,0.9,0.9,1)->conX.Left(20)->conY.Center(200)->conW.Center(200)->conH.Center(100);
		alone->NewTransition(&hideStart)->Move(-300, 0, 1)->Fade(63, 63, 107, 0, 0.8);
		alone->NewTransition(&alone->isHolding)->Fade(63/3, 63/3, 107/3, 1,0.15);
		alone->OnClick = [](int mx,int my,int button,int action) {
			hideStart = true;
			showAlone = true;
		};
		
		IElement* aloneScroll = NewElement("aloneScroll",0);
		aloneScroll->AddTag(&menu);
		aloneScroll->Color(100, 100, 100, 0)->conX.Left(20-300)->conY.Center(0.f)->conW.Center(260)->conH.Center(1.f);
		aloneScroll->NewTransition(&showAlone)->Move(300, 0, 1)->Fade(100, 100, 100, 0, 0.8);

		IElement* aloneBack = NewElement("aloneBack",2);
		aloneBack->AddTag(&menu);
		aloneBack->Color(63, 63, 107, 0)->Text(GetFont(), "Back", 30,0.9, 0.9, 0.9, 1)->conX.Right(20-250)->conY.Bottom(20)->conW.Center(200)->conH.Center(100);
		aloneBack->NewTransition(&showAlone)->Move(-250, 0, 1)->Fade(63, 63, 107, 1, 0.8);
		aloneBack->NewTransition(&aloneBack->isHolding)->Fade(63 / 3, 63 / 3, 107 / 3, 1, 0.15);
		aloneBack->OnClick = [](int mx, int my, int button, int action) {
			hideStart = false;
			showAlone = false;
			editWorld = false;
		};

		IElement* editDesc = NewElement("editDesc",1);
		editDesc->AddTag(&menu);
		editDesc->Color(63, 63, 107, 0)->Text(GetFont(), "Description\n of this world\nPlay Time: 0s\n...", 0.9, 0.9, 0.9, 1)->conX.Left(aloneScroll, 20)->conY.Top(20)->conW.Center(260)->conH.Center(0.5f);
		editDesc->NewTransition(&editWorld)->Fade(63, 63, 107, 1, 0.8);

		IElement* editPlay = NewElement("editPlay",1);
		editPlay->AddTag(&menu);
		editPlay->Color(63, 63, 107, 0)->Text(GetFont(), "Play!", 30, 0.9, 0.9, 0.9, 1)->conX.Left(aloneScroll, 20)->conY.Top(editDesc,10)->conW.Center(260)->conH.Center(80);
		editPlay->NewTransition(&editWorld)->Fade(63, 63, 107, 1, 0.8);
		editPlay->NewTransition(&editPlay->isHolding)->Fade(63 / 3, 63, 107 / 3, 1, 0.15);
		editPlay->OnClick = [=](int mx, int my,int button,int action) {
			//GetElement("aloneWorld" + std::to_string(editId))->SetText(editName->GetText());
			//bug::out < "Play" < bug::end;
			UIFadeOn();
			dManager::AddTimedFunction([=]() {
				// Load world data
				dManager::SetGameState(GameState::play);
				menu = false;
				UIFadeOff();
			}, 1.f);
		};

		IElement* last = nullptr;
		for (int i = 0; i < 5;i++) {
			IElement* world = NewElement("aloneWorld"+std::to_string(i),1);
			world->AddTag(&menu);
			world->Color(83, 83, 127, 0)->Text(GetFont(), "World Name "+std::to_string(i),30, 0.9, 0.9, 0.9, 1)->
				conX.Left(30-300)->conW.Center(240)->conH.Center(80);
			if (last == nullptr)
				world->conY.Top(30);
			else
				world->conY.Top(last, 10);
			
			world->NewTransition(&showAlone)->Move(300, 0, 0.5+i*0.05)->Fade(83, 83, 127, 1, 0.8);
			world->NewTransition(&world->isHolding)->Fade(30, 30, 90, 1, 0.15);
			world->NewTransition(&world->isHovering)->Move(10,0,0.2)->Fade(63, 63, 107, 1, 0.15);
			world->NewTransition(&world->isSelected)->Move(10,0,0.2)->Fade(83 / 3, 83, 127 / 3, 1, 0.15);
			world->OnClick = [=](int mx, int my, int button, int action) {
				//editName->SetText(world->GetText());
				editWorld = true;
				editId = i;
			};
			world->SetEditable();
			/*world->OnKey = [=](int key, int action) {
				GetElement("aloneWorld" + std::to_string(editId))->SetText(editName->GetText());
			};
			if (i == 0) {
				aloneScroll->OnScroll = [=](double scroll) {
					world->conY.Add((int)(scroll*40));
				};
			}
			last = world;
			
		}*/
	}
}