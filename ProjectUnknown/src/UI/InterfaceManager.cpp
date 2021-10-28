#include "propch.h"

#include "InterfaceManager.h"

#include "GameHandler.h"

#include "Items/ItemHandler.h"

namespace ui
{
	static Item* heldItem;
	void SetHeldItem(Item* item)
	{
		if (heldItem != nullptr) {
			delete heldItem;
		}
		heldItem = item;
	}
	Item* GetHeldItem()
	{
		return heldItem;
	}
	Item* TakeHeldItem()
	{
		Item* temp = heldItem;
		heldItem = nullptr;
		return temp;
	}
	Item** GetHeldItemPointer()
	{
		return &heldItem;
	}
	void Render(double lag)
	{
		/*
		if (inventory != nullptr) {:
			if (inventory->IsActive()) {
				float iw = 64 / 1920.f * 2;
				float ih = 64 / 1080.f * 2;

				float aspectRatio = 1080 / 1920.f;

				if (aspectRatio != 0) {
					float newRatio = engone::Height() / engone::Width();
					if (newRatio > aspectRatio) {
						// Height is bigger or width is smaller
						ih *= aspectRatio / newRatio;
					}
					else {
						// Width bigger or height smaller
						iw *= newRatio / aspectRatio;
					}
				}
				DrawItem(heldItem, engone::GetFloatMouseX() - iw / 2, engone::GetFloatMouseY() + ih / 2, iw, ih, 1, 1, 1, 1);
			}
		}
		*/
	}
	bool hasIntro = false;
	void SetupIntro()
	{
		if (hasIntro)
			return;
		hasIntro = true;
		using namespace engone;

		Panel* intro = new Panel("intro");

		intro->CenterX(0)->CenterY(0)->Width(1.f)->Height(1.f);
		AddPanel(intro);

		/*
		AddTexture("intro",new Texture("assets/textures/intro.png"));
		engone::AddBase(introScene = new IntroScene("IntroScene"));
		introScene->conX.Center(0.f)->conY.Center(0.f)
			->conW.Center(331 / 1920.f * 2)->conH.Center(277 / 1080.f * 2)->SetFixed();
		*/
	}
	// What to do with these?
	bool uiFadeBool = false;
	bool hideStart = false;
	bool showAlone = false;
	bool editWorld = false;
	int editId = 0;
	bool stateMenu()
	{
		return engone::CheckState(GameState::Menu);
	}
	bool hasMainMenu = false;
	void SetupMainMenu()
	{
		if (hasMainMenu)
			return;
		hasMainMenu = true;

		using namespace engone;
#if begone
		// Stage One
		IElement* alone = AddElement("playAlone", 5);
		{
			alone->AddTag(stateMenu);
			alone->Color({ 63, 63, 107 });
			alone->Text("consolas42", "Play Alone", 30, { 0.9f });
			alone->conX.Left(20)->conY.Top(0)->conW.Center(200)->conH.Center(100)->SetFixed();
			alone->NewTransition(&hideStart)->Move(-300, 0, 1)->Fade({ 63, 63, 107, 0 }, 0.8);
			alone->NewTransition(&alone->isHolding)->Fade({ 63 / 3, 63 / 3, 107 / 3, 255 }, 0.15);
			alone->OnMouse = [](int mx, int my, int button, int action) {
				hideStart = true;
				showAlone = true;
			};
		}
		// Settings
		// Quit

		// Stage Play
		/*AddBase(worldHandler = new WorldHandler("WorldHandler"));
		worldHandler->Color({ 0.4f,0.4f });
		worldHandler->conX.Left(30)->conY.Top(0.f)->conW.Center(260)->conH.Center(2.f)->SetFixed();
		*/
		IElement* aloneScroll = AddElement("aloneScroll", 0);
		{
			aloneScroll->AddTag(stateMenu);
			aloneScroll->Color({ 100, 0 });
			aloneScroll->conX.Left(20 - 300)->conY.Center(0.f)->conW.Center(260)->conH.Center(2.f)->SetFixed();
			aloneScroll->NewTransition(&showAlone)->Move(300, 0, 1);//->Fade({ 100, 0 }, 0.8);
		}
		IElement* aloneBack = AddElement("aloneBack", 2);
		{
			aloneBack->AddTag(stateMenu);
			aloneBack->Color({ 63, 63, 107, 0 });
			aloneBack->Text("consolas42", "Back", 30, { 0.9f });
			aloneBack->conX.Right(20 - 250)->conY.Bottom(20)->conW.Center(200)->conH.Center(100)->SetFixed();
			aloneBack->NewTransition(&showAlone)->Move(-250, 0, 1)->Fade({ 63, 63, 107, 255 }, 0.8);
			aloneBack->NewTransition(&aloneBack->isHolding)->Fade({ 63 / 3, 63 / 3, 107 / 3, 255 }, 0.15);
			aloneBack->OnMouse = [](int mx, int my, int button, int action) {
				hideStart = false;
				showAlone = false;
				editWorld = false;
			};
		}
		IElement* editDesc = AddElement("editDesc", 1);
		{
			editDesc->AddTag(stateMenu);
			editDesc->Color({ 63, 63, 107, 0 });
			editDesc->Text("consolas42", "Description\n of this world\nPlay Time: 0s\n...", { 0.9f });
			editDesc->conX.Left(aloneScroll, 20)->conY.Top(20)->conW.Center(260)->conH.Center(1.f)->SetFixed();
			editDesc->NewTransition(&editWorld)->Fade({ 63, 63, 107, 255 }, 0.8);
		}
		IElement* editPlay = AddElement("editPlay", 1);
		{
			editPlay->AddTag(stateMenu);
			editPlay->Color({ 63, 63, 107, 0 });
			editPlay->Text("consolas42", "Play!", 30, { 0.9f });
			editPlay->conX.Left(aloneScroll, 20)->conY.Top(editDesc, 10)->conW.Center(260)->conH.Center(80)->SetFixed();
			editPlay->NewTransition(&editWorld)->Fade({ 63, 63, 107, 255 }, 0.8);
			editPlay->NewTransition(&editPlay->isHolding)->Fade({ 63 / 3, 63, 107 / 3, 255 }, 0.15);
			editPlay->OnMouse = [=](int mx, int my, int button, int action) {
				//GetElement("aloneWorld" + std::to_string(editId))->SetText(editName->GetText());

				uiFadeBool = true;
				AddTimedFunction([=]() {
					// Load world data

					game::StartGame();

					uiFadeBool = false;
					if (infobar != nullptr)
						infobar->active = true;
					if (hotbar != nullptr)
						hotbar->active = true;
					}, 1.f);
			};
		}
		IElement* last = nullptr;
		for (int i = 0; i < 5; i++) {
			IElement* world = AddElement("aloneWorld" + std::to_string(i), 1);
			world->AddTag(stateMenu);
			world->Color({ 83, 83, 127, 0 });
			world->Text("consolas42", "World Name " + std::to_string(i), 30, { 0.9f });
			world->conX.Left(30 - 300)->conW.Center(240)->conH.Center(80)->SetFixed();
			if (last == nullptr)
				world->conY.Top(30);
			else
				world->conY.Top(last, 30);

			world->NewTransition(&showAlone)->Move(300, 0, 0.5 + i * 0.05)->Fade({ 83, 83, 127, 255 }, 0.8);
			world->NewTransition(&world->isHolding)->Fade({ 30, 30, 90, 255 }, 0.15);
			world->NewTransition(&world->isHovering)->Move(10, 0, 0.2)->Fade({ 63, 63, 107, 255 }, 0.15);
			world->NewTransition(&world->isSelected)->Move(10, 0, 0.2)->Fade({ 83 / 3, 83, 127 / 3, 255 }, 0.15);
			world->OnMouse = [=](int mx, int my, int button, int action) {
				//editName->SetText(world->GetText());
				editWorld = true;
				editId = i;
			};
			//world->SetEditable();
			/*world->OnKey = [=](int key, int action) {GetElement("aloneWorld" + std::to_string(editId))->SetText(editName->GetText());};*/

			if (i == 0) {
				aloneScroll->OnScroll = [=](double scroll) {
					world->conY.Add((int)(scroll * 40));
				};
			}
			last = world;
		}
		IElement* background = AddElement("background", -5);
		{
			background->AddTag(stateMenu);
			background->Color({ 255,255,255,255 });
			background->conX.Center(0.f)->conY.Center(0.f)->conW.Center(2.f)->conH.Center(2.f);
		}
		uiFade = AddElement("uiFade", 50);
		{
			uiFade->Color({ 0.f,0.f });
			uiFade->conX.Center(0)->conY.Center(0)->conW.Center(2.f)->conH.Center(2.f);
			uiFade->NewTransition(&uiFadeBool)->Fade({ 0.f, 1.f }, .5f);
		}
#endif
	}
	bool hasGameUI = false;
	void SetupGameUI()
	{
		if (hasGameUI)
			return;
		hasGameUI = true;
		using namespace engone;
		engone::AddAsset<engone::Texture>("inventory", "textures/containers/inventory");

		/*

		engone::AddBase(inventory = new Inventory("PlayerInventory"));
		inventory->conX.Right(0.f)->conY.Center(0.f)
			->conW.Center(440 / 1920.f * 2)->conH.Center(850 / 1080.f * 2)->SetFixed();

		engone::AddTexture("craftinglist",new engone::Texture("assets/textures/containers/craftinglist.png"));
		engone::AddBase(craftingList = new CraftingList("PlayerCrafting"));
		craftingList->conX.Right(inventory, 0.01f)->conY.Center(0.f)
			->conW.Center(512 / 1920.f * 2)->conH.Center(850 / 1080.f * 2)->SetFixed();

		engone::AddTexture("infobar",new engone::Texture("assets/textures/containers/infobar"));
		engone::AddBase(infobar = new Infobar("PlayerInfobar"));
		infobar->conX.Left(0.f)->conY.Top(0.f)
			->conW.Center(550 / 1920.f)->conH.Center(226 / 1080.f)->SetFixed();

		engone::AddTexture("hotbar",new engone::Texture("assets/textures/containers/hotbar"));
		engone::AddBase(hotbar = new Hotbar("PlayerHotbar"));
		hotbar->conX.Center(0.f)->conY.Bottom(0.02f)
			->conW.Center(608 / 1920.f * 2)->conH.Center(64 / 1080.f * 2)->SetFixed();
		*/
	}
	/*
	bool pause = false;
	void InitPauseMenu() {
		using namespace engine;
		pause = true;// GetPauseMode();
		IElement* back = AddElement("pauseBack", 0);
		{
			back->AddTag([=]() {return CheckState(GameState::Game); });
			back->Color({ 50 });
			back->Text(GetFont(), "Back", 30, { 0.9f });
			back->conX.Left(.02f)->conY.Center(0.f)->conW.Center(200)->conH.Center(100)->SetFixed();
			back->NewTransition([=]() {return !GetPauseMode(); })->Move(-200, 0, 1)->Fade({ 50, 0 }, 0.8);
			back->NewTransition(&back->isHolding)->Fade({ 50 / 3 }, 0.15);
			back->OnMouse = [](int mx, int my, int button, int action) {
				uiFadeBool=true;
				AddTimedFunction([=]() {
					SetState(GameState::Game,false);
					SetState(GameState::Menu,true);
				uiFadeBool = false;
				}, 1.f);
			uiInventory->active = false;
			uiInfobar->active = false;
			uiHotbar->active = false;
			uiCrafting->active = false;
		};
	}
}*/
}