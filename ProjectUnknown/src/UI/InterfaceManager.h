#pragma once

#include "Inventory.h"
#include "CraftingList.h"
#include "Infobar.h"
#include "Hotbar.h"

namespace UI {

	Inventory* GetInventory();
	CraftingList* GetCraftingList();
	Infobar* GetInfobar();
	Hotbar* GetHotbar();

	void InitializeUI();
	
	void SetHeldItem(Item* item);
	Item* GetHeldItem();
	Item* TakeHeldItem();
	Item** GetHeldItemPointer();

	void Render();

	void InitStartMenu();
	void InitPauseMenu();
	void InitGameMenu();
}