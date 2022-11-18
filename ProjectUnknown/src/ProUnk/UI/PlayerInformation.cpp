#include "ProUnk/UI/PlayerInformation.h"

#include "Engone/Engone.h"

#include "ProUnk/Combat/CombatData.h"

namespace prounk {
	void RenderPlayerInformation(engone::LoopInfo& info, PlayerController* playerController) {
		using namespace engone;
		float sw = GetWidth();
		float sh = GetHeight();

		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		//-- health
		//{
		int border = 4;

		ui::Box healthBack = { 10,10,sw * 0.2,50,{0.5f,0.5f,0.5f,0.5f} };
		ui::Draw(healthBack);

		
		CombatData* combatData = playerController->m_world->entityHandler.getEntry(playerController->getPlayerObject()->userData).combatData;

		float& health = combatData->health;
		float maxHealth = combatData->getMaxHealth();

		ui::Box healthFront = { healthBack.x + border,healthBack.y + border,
			(healthBack.w - border * 2) * health / maxHealth,
			healthBack.h - border * 2,{1.f,0.01f,0.02f,1.f} };
		ui::Draw(healthFront);

		char str[20];
		memset(str, 0, sizeof(str));
		snprintf(str, 20, "%d/%d", (int)health, (int)maxHealth);

		ui::TextBox healthText = { str,healthFront.x,healthFront.y,healthFront.h,consolas,{1.f,1.f,1.f,1.f} };
		ui::Draw(healthText);
		//}
		//-- attack
		//{
		//int border = 4;

		ui::Box atkBack = { 10,healthBack.y+healthBack.h+10,sw * 0.2,50,{0.5f,0.5f,0.5f,0.5f} };
		ui::Draw(atkBack);

		float atk = combatData->getAttack();
		//float maxHealth = player->combatData.getMaxHealth();

		//ui::Box healthFront = { back.x + border,back.y + border,
		//	(back.w - border * 2) * health / maxHealth,
		//	healthBack.h - border * 2,{1.f,0.01f,0.02f,1.f} };
		//ui::Draw(healthFront);

		//char str[20];
		memset(str, 0, sizeof(str));
		snprintf(str, 20, "%d", (int)atk);

		ui::TextBox atkText = { str,atkBack.x,atkBack.y,atkBack.h,consolas,{1.f,1.f,1.f,1.f} };
		ui::Draw(atkText);
		//}
	}
}