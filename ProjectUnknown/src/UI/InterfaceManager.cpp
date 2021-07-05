#include "InterfaceManager.h"

#include "Inventory.h"
#include "CraftingList.h"
#include "Infobar.h"
#include "Hotbar.h"

#include "Items/ItemHandler.h"

#include "GameStates.h"


namespace UI {
	Inventory* uiInventory;
	Inventory* GetInventory() {
		return uiInventory;
	}
	CraftingList* uiCrafting;
	CraftingList* GetCraftingList() {
		return uiCrafting;
	}
	Infobar* uiInfobar;
	Infobar* GetInfobar() {
		return uiInfobar;
	}
	Hotbar* uiHotbar;
	Hotbar* GetHotbar() {
		return uiHotbar;
	}
	void InitializeUI() {
		InitStartMenu();
		InitPauseMenu();
		InitGameMenu();

		engine::AddTextureAsset("containers/inventory");
		engine::NewBase(uiInventory = new Inventory("PlayerInventory"));
		uiInventory->conX.Right(0.f)->conY.Center(0.f)
			->conW.Center(440 / 1920.f)->conH.Center(850 / 1080.f)->SetFixed();
		
		engine::AddTextureAsset("containers/craftinglist");
		engine::NewBase(uiCrafting = new CraftingList("PlayerCrafting"));
		uiCrafting->conX.Right(uiInventory,0.005f)->conY.Center(0.f)
			->conW.Center(512 / 1920.f)->conH.Center(850 / 1080.f)->SetFixed();
			
		engine::AddTextureAsset("containers/infobar");
		engine::NewBase(uiInfobar = new Infobar("PlayerInfobar"));
		uiInfobar->conX.Left(0.f)->conY.Top(0.f)->conW.Center(550 / 1920.f / 2 )->conH.Center(226 / 1080.f / 2 )->SetFixed();
		
		engine::AddTextureAsset("containers/hotbar");
		engine::NewBase(uiHotbar = new Hotbar("PlayerHotbar"));
		uiHotbar->conX.Center(0.f)->conY.Bottom(0.02f)->conW.Center(608 / 1920.f)->conH.Center(64 / 1080.f)->SetFixed();
		
	}
	Item* heldItem;
	void SetHeldItem(Item* item) {
		if (heldItem != nullptr) {
			delete heldItem;
		}
		heldItem = item;
	}
	Item* GetHeldItem() {
		return heldItem;
	}
	Item* TakeHeldItem() {
		Item* temp = heldItem;
		heldItem = nullptr;
		return temp;
	}
	Item** GetHeldItemPointer() {
		return &heldItem;
	}
	void Render() {
		if (uiInventory != nullptr) {
			if (uiInventory->IsActive()) {
				float iw = 64 / 1920.f * 2;
				float ih = 64 / 1080.f * 2;

				float aspectRatio = 1080 / 1920.f;

				if (aspectRatio != 0) {
					float newRatio = engine::Height() / engine::Width();
					if (newRatio > aspectRatio) {
						// Height is bigger or width is smaller
						ih *= aspectRatio / newRatio;
					}
					else {
						// Width bigger or height smaller
						iw *= newRatio / aspectRatio;
					}
				}
				DrawItem(heldItem, engine::GetFloatMouseX() - iw / 2, engine::GetFloatMouseY() + ih / 2, iw, ih, 1, 1, 1, 1);
			}
		}
	}
	// What to do with these?
	bool hideStart = false;
	bool showAlone = false;
	bool editWorld = false;
	int editId = 0;
	bool stateMenu() {
		return IsGameState(Menu);
	}
	void InitStartMenu() {
		using namespace engine;

		/*
		IElement* red = NewElement("red");
		red->Color(1.f, 0.f, 0.f, 1)->Text(GetFont(),"This is\n a lot of\n experimental text\n as you can see",30,1,1,1,1)->
			conX.Center(0)->conY.Center(0)->conW.Center(0.25f)->conH.Center(100);
		red->NewTransition(&hideAlone)->Fade(1.f, 0.f,0.f, 0.f,2.f);
		*/
		IElement* alone = NewElement("playAlone", 5);
		{
			alone->AddTag(stateMenu);
			alone->Color({ 63, 63, 107 });
			alone->Text(GetFont(), "Play Alone", 30, { 0.9f });
			alone->conX.Left(20)->conY.Top(0)->conW.Center(200)->conH.Center(100)->SetFixed();
			alone->NewTransition(&hideStart)->Move(-300, 0, 1)->Fade({ 63, 63, 107, 0 }, 0.8);
			alone->NewTransition(&alone->isHolding)->Fade({ 63 / 3, 63 / 3, 107 / 3, 1 }, 0.15);
			alone->OnClick = [](int mx, int my, int button, int action) {
				hideStart = true;
				showAlone = true;
			};
		}
		IElement* aloneScroll = NewElement("aloneScroll", 0);
		{
			aloneScroll->AddTag(stateMenu);
			aloneScroll->Color({ 100, 0 });
			aloneScroll->conX.Left(20 - 300)->conY.Center(0.f)->conW.Center(260)->conH.Center(1.f)->SetFixed();
			aloneScroll->NewTransition(&showAlone)->Move(300, 0, 1)->Fade({ 100, 0 }, 0.8);
		}
		IElement* aloneBack = NewElement("aloneBack", 2);
		{
			aloneBack->AddTag(stateMenu);
			aloneBack->Color({ 63, 63, 107, 0 });
			aloneBack->Text(GetFont(), "Back", 30, { 0.9f });
			aloneBack->conX.Right(20 - 250)->conY.Bottom(20)->conW.Center(200)->conH.Center(100)->SetFixed();
			aloneBack->NewTransition(&showAlone)->Move(-250, 0, 1)->Fade({ 63, 63, 107, 1 }, 0.8);
			aloneBack->NewTransition(&aloneBack->isHolding)->Fade({ 63 / 3, 63 / 3, 107 / 3, 1 }, 0.15);
			aloneBack->OnClick = [](int mx, int my, int button, int action) {
				hideStart = false;
				showAlone = false;
				editWorld = false;
			};
		}
		IElement* editDesc = NewElement("editDesc", 1);
		{
			editDesc->AddTag(stateMenu);
			editDesc->Color({ 63, 63, 107, 0 });
			editDesc->Text(GetFont(), "Description\n of this world\nPlay Time: 0s\n...", { 0.9f });
			editDesc->conX.Left(aloneScroll, 20)->conY.Top(20)->conW.Center(260)->conH.Center(0.5f)->SetFixed();
			editDesc->NewTransition(&editWorld)->Fade({ 63, 63, 107, 1 }, 0.8);
		}

		IElement* editPlay = NewElement("editPlay", 1);
		{
			editPlay->AddTag(stateMenu);
			editPlay->Color({ 63, 63, 107, 0 });
			editPlay->Text(GetFont(), "Play!", 30, { 0.9f });
			editPlay->conX.Left(aloneScroll, 20)->conY.Top(editDesc, 10)->conW.Center(260)->conH.Center(80)->SetFixed();
			editPlay->NewTransition(&editWorld)->Fade({ 63, 63, 107, 1 }, 0.8);
			editPlay->NewTransition(&editPlay->isHolding)->Fade({ 63 / 3, 63, 107 / 3, 1 }, 0.15);
			editPlay->OnClick = [=](int mx, int my, int button, int action) {
				//GetElement("aloneWorld" + std::to_string(editId))->SetText(editName->GetText());
				//bug::out < "Play" < bug::end;
				UIFadeOn();
				AddTimedFunction([=]() {
					// Load world data
					SetGameState(Play);
					UIFadeOff();
					uiInfobar->active = true;
					uiHotbar->active = true;
					}, 1.f);
			};
		}

		IElement* last = nullptr;
		for (int i = 0; i < 5; i++) {
			IElement* world = NewElement("aloneWorld" + std::to_string(i), 1);
			world->AddTag(stateMenu);
			world->Color({ 83, 83, 127, 0 });
			world->Text(GetFont(), "World Name " + std::to_string(i), 30, { 0.9f });
			world->conX.Left(30 - 300)->conW.Center(240)->conH.Center(80)->SetFixed();
			if (last == nullptr)
				world->conY.Top(30);
			else
				world->conY.Top(last, 30);

			world->NewTransition(&showAlone)->Move(300, 0, 0.5 + i * 0.05)->Fade({ 83, 83, 127, 1 }, 0.8);
			world->NewTransition(&world->isHolding)->Fade({ 30, 30, 90, 1 }, 0.15);
			world->NewTransition(&world->isHovering)->Move(10, 0, 0.2)->Fade({ 63, 63, 107, 1 }, 0.15);
			world->NewTransition(&world->isSelected)->Move(10, 0, 0.2)->Fade({ 83 / 3, 83, 127 / 3, 1 }, 0.15);
			world->OnClick = [=](int mx, int my, int button, int action) {
				//editName->SetText(world->GetText());
				editWorld = true;
				editId = i;
			};
			world->SetEditable();
			/*world->OnKey = [=](int key, int action) {
				GetElement("aloneWorld" + std::to_string(editId))->SetText(editName->GetText());
			};*/
			if (i == 0) {
				aloneScroll->OnScroll = [=](double scroll) {
					world->conY.Add((int)(scroll * 40));
				};
			}
			last = world;
		}
		IElement* background = NewElement("background",-5);
		background->AddTag(stateMenu);
		background->Color({255,255,255,255});
		background->conX.Center(0.f)->conY.Center(0.f)->conW.Center(2.f)->conH.Center(2.f);
	}
	bool pause = false;

	void InitPauseMenu() {
		using namespace engine;

		pause = true;// GetPauseMode();

		IElement* back = NewElement("pauseBack", 0);
		{
			back->AddTag([=]() {return IsGameState(Play); });
			back->Color({ 50 });
			back->Text(GetFont(), "Back", 30, { 0.9f });
			back->conX.Left(.01f)->conY.Center(0.f)->conW.Center(200)->conH.Center(100)->SetFixed();
			back->NewTransition([=]() {return !GetPauseMode(); })->Move(-200, 0, 1)->Fade({ 50, 0 }, 0.8);
			back->NewTransition(&back->isHolding)->Fade({ 50 / 3 }, 0.15);
			back->OnClick = [](int mx, int my, int button, int action) {
				UIFadeOn();
				AddTimedFunction([=]() {
					SetGameState(Menu);
					UIFadeOff();
					}, 1.f);
				uiInventory->active = false;
				uiInfobar->active = false;
				uiHotbar->active = false;
				uiCrafting->active = false;
			};
		}
	}
	void InitGameMenu() {
		using namespace engine;

		IElement* chatBox = NewElement("chatBox", 0);
		{
			chatBox->AddTag([=]() {return IsKey(GLFW_KEY_T); });
			chatBox->Color({ 50 });
			chatBox->Text(GetFont(), "Chatbox", 30, { 0.9f });
			chatBox->conX.Left(20)->conY.Bottom(20)->conW.Center(0.25f)->conH.Center(0.33f)->SetFixed();
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