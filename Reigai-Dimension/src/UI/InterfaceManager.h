#pragma once

#include "Items/Item.h"

namespace UI {
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