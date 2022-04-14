#pragma once
/*
#include "Inventory.h"
#include "WorldHandler.h"
#include "CraftingList.h"
#include "Infobar.h"
#include "Hotbar.h"
#include "IntroScene.h"
*/
#include "Items/Item.h"
//#include "Engone/UI/GuiHandler.h"

#include "GameStateEnum.h"

namespace UI
{
	void SetHeldItem(Item* item);
	Item* GetHeldItem();
	Item* TakeHeldItem();
	Item** GetHeldItemPointer();

	void Render(double lag);

	void SetupIntro();
	void SetupMainMenu();
	void SetupGameUI();
}
/*
class InterfaceManager {
private:
public:
	InterfaceManager();
	~InterfaceManager();// Does nothing

	// Menu ui
	//WorldHandler* worldHandler=nullptr;

	// Check if nullptr
	//engone::IElement* uiFade = nullptr;

	// Game ui
	// Check if nullptr
	//IntroScene* introScene=nullptr;Inventory* inventory=nullptr;CraftingList* craftingList=nullptr;Infobar* infobar=nullptr;Hotbar* hotbar=nullptr;

	Item* heldItem;
	void SetHeldItem(Item* item);
	Item* GetHeldItem();
	Item* TakeHeldItem();
	Item** GetHeldItemPointer();

	void Render(double lag);

	void SetupIntro();
	void SetupMainMenu();
	void SetupGameUI();
};
extern InterfaceManager interfaceManager;
*/