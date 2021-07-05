#include "Infobar.h"

#include "GLFW/glfw3.h"

#include "GameHandler.h"

Infobar::Infobar(const std::string& name) : engine::IBase(name) {
	active = true;
}
bool Infobar::IsActive() {
	return active;
}
void Infobar::Update(float delta) {
	if (IsActive()) {

	}
}
void Infobar::Render() {
	//bug::out < "Invetory Draw" < bug::end;
	if (IsActive()) {
		// Inventory background
		engine::BindTexture(0, "containers/infobar");

		CalcConstraints();

		engine::DrawRect(x, y, w, h, color.r, color.g, color.b, color.a);
	
		Player* plr = GetPlayer();

		float health = plr->health / plr->maxHealth;
		float stamina = plr->stamina / plr->maxStamina;
		float hunger = plr->hunger / plr->maxHunger;
		float mana = plr->mana / plr->maxMana;

		float barHeight = h * 48/226.f;
		float barGap = h * 10/226.f;
		float fullWidth = w * 546/550.f;
		float barBorderW = w * 2 / 550.f;
		float barBorderH = h * 2 / 226.f;

		engine::BindTexture(0, "blank");

		float baseX = x + barBorderW;
		float baseY = y + h - barBorderH - barHeight;
		float strideY = (barHeight+barGap);
		
		engine::GuiColor(0, 1, 0, 1);
		engine::DrawRect(baseX, baseY,health * fullWidth,barHeight);
		
		engine::GuiColor(1, 1, 0, 1);
		engine::DrawRect(baseX, baseY - strideY,stamina * fullWidth,barHeight);
		
		engine::GuiColor(0.8, 0.5, 0.2, 1);
		engine::DrawRect(baseX, baseY - 2*strideY,hunger * fullWidth,barHeight);
		
		engine::GuiColor(0.1, 0.2, 1, 1);
		engine::DrawRect(baseX, baseY - 3*strideY,mana * fullWidth,barHeight);
		
		engine::GuiColor(1, 1, 1, 1);
		engine::GuiSize(1, 1);
		
		float textX = x + fullWidth/2 + barBorderW;
		float textY = y + h - barBorderH - barHeight/2;

		engine::GuiTransform(textX, textY);
		engine::DrawString(engine::GetFont(), std::to_string((int)plr->health) + "/" + std::to_string((int)plr->maxHealth), true, barHeight, -1);
	
		engine::GuiTransform(textX, textY - strideY);
		engine::DrawString(engine::GetFont(), std::to_string((int)plr->stamina) + "/" + std::to_string((int)plr->maxStamina), true, barHeight, -1);
		
		engine::GuiTransform(textX, textY - 2*strideY);
		engine::DrawString(engine::GetFont(), std::to_string((int)plr->hunger) + "/" + std::to_string((int)plr->maxHunger), true, barHeight, -1);
		
		engine::GuiTransform(textX, textY - 3*strideY);
		engine::DrawString(engine::GetFont(), std::to_string((int)plr->mana) + "/" + std::to_string((int)plr->maxMana), true, barHeight, -1);
		
	}
}