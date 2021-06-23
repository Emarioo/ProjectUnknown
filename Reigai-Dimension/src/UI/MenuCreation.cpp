#include "MenuCreation.h"

#include "Inventory.h"
#include "InfoBar.h"
#include "Hotbar.h"
#include "Engine/Rendering/Renderer.h"

Inventory* uiInventory;
InfoBar* uiInfoBar;
Hotbar* uiHotbar;
namespace UI {
	void InitializeUI() {
		InitStartMenu();
		InitPauseMenu();
		InitGameMenu();
		
		engine::AddTextureAsset("containers/inventory");
		engine::NewBase(uiInventory=new Inventory("PlayerInventory"));
		uiInventory->conX.Right(0.f)->conY.Center(0.f)
			->conW.Center(440/1920.f)->conH.Center(850/1080.f)->SetFixed();

		engine::AddTextureAsset("containers/infobar");
		engine::NewBase(uiInfoBar = new InfoBar("PlayerInfoBar"));
		uiInfoBar->conX.Left(0.f)->conY.Top(0.f)->conW.Center(550 / 1920.f/2)->conH.Center(226 / 1080.f/2)->SetFixed();
		
		engine::AddTextureAsset("containers/hotbar");
		engine::NewBase(uiHotbar = new Hotbar("PlayerHotbar"));
		uiHotbar->conX.Center(0.f)->conY.Bottom(0.1f)->conW.Center(550 / 1920.f / 2)->conH.Center(226 / 1080.f / 2)->SetFixed();
	}
}