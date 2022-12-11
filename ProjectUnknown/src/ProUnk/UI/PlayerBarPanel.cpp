#include "ProUnk/UI/PlayerBarPanel.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	void PlayerBarPanel::render(engone::LoopInfo& info) {
		using namespace engone;
		Renderer* renderer = info.window->getRenderer();
		PlayerController* playerController = &m_app->playerController;
		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");
		CombatData* combatData = playerController->m_world->entityRegistry.getEntry(playerController->getPlayerObject()->userData).combatData;
		float& health = combatData->health;
		float maxHealth = combatData->getMaxHealth();
		float atk = combatData->getAttack();

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
		snprintf(str, 20, "%d/%d", (int)health, (int)maxHealth);

		ui::TextBox healthText = { str,healthBack.x,healthBack.y,healthBack.h,consolas,{1.f,1.f,1.f,1.f} };
		healthText.x = healthBack.x + healthBack.w / 2 - healthText.getWidth() / 2;
		ui::Draw(healthText);

		//-- attack
		ui::Box atkBack = { healthBack.x,healthBack.y + healthBack.h + barGap,area.w,barHeight,{0.5f,0.5f,0.5f,0.5f} };
		ui::Draw(atkBack);

		//char str[20];
		memset(str, 0, sizeof(str));
		snprintf(str, 20, "atk: %d", (int)atk);

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
	}
}