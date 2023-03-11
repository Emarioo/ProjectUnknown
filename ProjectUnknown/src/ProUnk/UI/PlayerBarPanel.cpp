#include "ProUnk/UI/PlayerBarPanel.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/UI/InvUtility.h"

#include "ProUnk/Objects/BasicObjects.h"

namespace prounk {

	void PlayerBarPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		CommonRenderer* renderer = info.window->getCommonRenderer();
		//Renderer* renderer = info.window->getRenderer();
		PlayerController* playerController = &m_app->playerController;
		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		EngineObject* plr = playerController->requestPlayer();
		CombatData* combatData = GetCombatData(plr);
		
		float health = 0;
		float maxHealth = 0;
		if (combatData) {
			health = combatData->getHealth();
			maxHealth = combatData->getMaxHealth();
		}
		float atk = 0;
		// Todo: displaying atk of weapon here is temporary and should be removed.
		//		It is useful for debugging so display it in some debug menu.
		//EngineObject* weapon = playerController->requestHeldObject();
		//
		//if (weapon) {
		//	CombatData* weaponData = GetCombatData(weapon);
		//	atk = weaponData->singleDamage;
		//}

		//playerController->releasePlayer(plr);
		//playerController->releaseHeldObject(weapon);

		//-- panel background
		ui::Box area = getBox();
		area.rgba = { 0.4,0.5,0.6,0.5 };
		ui::Draw(area);

		//-- Colors & Settings
		ui::Color darkRed = {.5,0.05,0.08,1};
		ui::Color brightRed = {.95,0.05,0.08,1};

		float barHeight = 27;
		float barGap = 8;
		//float barStride = barHeight + barGap;

		char buffer[30];
		
		//-- Health
		ui::Box healthBack = { area.x,area.y,area.w,barHeight,darkRed };

		ui::Box healthFront = { healthBack.x,healthBack.y,
			(healthBack.w) * health / maxHealth,
			healthBack.h, brightRed };

		snprintf(buffer, 20, "%.2f/%.2f", health, maxHealth);

		ui::TextBox healthText = { buffer,0,0,healthBack.h,consolas,{1.f,1.f,1.f,1.f} };
		healthText.x = healthBack.x + healthBack.w / 2 - healthText.getWidth() / 2;
		healthText.y = healthBack.y;

		//-- Attack
		ui::Box atkBack = { healthBack.x,healthBack.y + healthBack.h + barGap,area.w,barHeight,{0.5f,0.5f,0.5f,0.5f} };

		snprintf(buffer, 20, "atk: %.2f", atk);

		ui::TextBox atkText = { buffer,atkBack.x,atkBack.y,atkBack.h,consolas,{1.f,1.f,1.f,1.f} };
		atkText.x = atkBack.x + atkBack.w / 2 - atkText.getWidth()/2;

		//-- Drawing
		ui::Draw(healthBack);
		ui::Draw(healthFront);
		ui::Draw(healthText);
		ui::Draw(atkBack);
		ui::Draw(atkText);

		DrawToolTip(area.x + area.w, area.y, 20, 20, "Hello, random tooltip here.");

		float healthWidth = healthText.getWidth();
		float atkWidth = atkText.getWidth();
		float maxWidth=healthWidth;
		if (maxWidth < atkWidth)
			maxWidth = atkWidth;

		setMinWidth(maxWidth);
		setMinHeight(healthBack.h + barGap + atkBack.h);
	}
}