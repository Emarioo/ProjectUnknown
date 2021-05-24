#include "MenuCreation.h"

#include "Inventory.h"

Inventory* uiInventory;
namespace UI {
	void InitializeUI() {
		InitStartMenu();
		InitPauseMenu();
		InitGameMenu();
		
		engine::NewSystem(uiInventory=new Inventory("PlayerInventory"));
	}
}