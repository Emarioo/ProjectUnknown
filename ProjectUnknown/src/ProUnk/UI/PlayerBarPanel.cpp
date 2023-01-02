#include "ProUnk/UI/PlayerBarPanel.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/UI/InvUtility.h"

namespace prounk {

	void PlayerBarPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		Renderer* renderer = info.window->getRenderer();
		PlayerController* playerController = &m_app->playerController;
		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		CombatData* combatData = GetCombatData(playerController->app->getActiveSession(), playerController->getPlayerObject());
		
		float& health = combatData->health;
		float maxHealth = combatData->getMaxHealth();
		float atk = 0;
		// Todo: displaying atk of weapon here is temporary and should be removed.
		//		It is useful for debugging so display it in some debug menu.
		EngineObject* weapon = playerController->heldObject;
		if (weapon) {
			CombatData* weaponData = GetCombatData(playerController->app->getActiveSession(), weapon);
			atk = weaponData->singleDamage;
		}

		//-- panel background
		ui::Box area = getBox();
		area.rgba = { 0.4,0.5,0.6,1 };
		ui::Draw(area);

		float barHeight = 45;
		float barGap = 8;
		//float barStride = barHeight + barGap;

		//-- health
		ui::Color darkRed = {.5,0.05,0.08,1};
		ui::Color brightRed = {.95,0.05,0.08,1};
		ui::Box healthBack = { area.x,area.y,area.w,barHeight,darkRed };
		ui::Draw(healthBack);

		ui::Box healthFront = { healthBack.x,healthBack.y,
			(healthBack.w) * health / maxHealth,
			healthBack.h, brightRed };
		ui::Draw(healthFront);

		char str[20];
		memset(str, 0, sizeof(str));
		snprintf(str, 20, "%.2f/%.2f", health, maxHealth);

		ui::TextBox healthText = { str,healthBack.x,healthBack.y,healthBack.h,consolas,{1.f,1.f,1.f,1.f} };
		healthText.x = healthBack.x + healthBack.w / 2 - healthText.getWidth() / 2;
		ui::Draw(healthText);

		//-- attack
		ui::Box atkBack = { healthBack.x,healthBack.y + healthBack.h + barGap,area.w,barHeight,{0.5f,0.5f,0.5f,0.5f} };
		ui::Draw(atkBack);

		//char str[20];
		memset(str, 0, sizeof(str));
		snprintf(str, 20, "atk: %.2f", atk);

		ui::TextBox atkText = { str,atkBack.x,atkBack.y,atkBack.h,consolas,{1.f,1.f,1.f,1.f} };
		atkText.x = atkBack.x + atkBack.w / 2 - atkText.getWidth()/2;
		ui::Draw(atkText);

		setMinHeight(healthBack.h + barGap + atkBack.h);

		float healthWidth = healthText.getWidth();
		float atkWidth = atkText.getWidth();
		float maxWidth=healthWidth;
		if (maxWidth < atkWidth)
			maxWidth = atkWidth;
		setMinWidth(maxWidth);

		DrawToolTip(area.x + area.w, area.y, 20, 20, "Hello, random tooltip here.");
	}
}