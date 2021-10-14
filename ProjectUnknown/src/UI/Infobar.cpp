#include "propch.h"

#include "Infobar.h"

#include "GameHandler.h"

#include "Engone/Rendering/Renderer.h"

Infobar::Infobar(const std::string& name) : engone::IBase(name) {
	active = true;
}
void Infobar::Update(float delta) {
	
}
void Infobar::Render() {
	// Inventory background
	engone::BindTexture(0, "containers/infobar");

	engone::DrawRect(x, y, w, h, color.r, color.g, color.b, color.a);

	Player* plr = game::GetPlayer();

	float health = plr->health / plr->maxHealth;
	float stamina = plr->stamina / plr->maxStamina;
	float hunger = plr->hunger / plr->maxHunger;
	float mana = plr->mana / plr->maxMana;

	float barHeight = h * 48 / 226.f;
	float barGap = h * 10 / 226.f;
	float fullWidth = w * 546 / 550.f;
	float barBorderW = w * 2 / 550.f;
	float barBorderH = h * 2 / 226.f;

	engone::BindTexture(0, "blank");

	float baseX = x + barBorderW;
	float baseY = y + h - barBorderH - barHeight;
	float strideY = (barHeight + barGap);

	engone::SetColor(0, 1, 0, 1);
	engone::DrawRect(baseX, baseY, health * fullWidth, barHeight);

	engone::SetColor(1, 1, 0, 1);
	engone::DrawRect(baseX, baseY - strideY, stamina * fullWidth, barHeight);

	engone::SetColor(0.8, 0.5, 0.2, 1);
	engone::DrawRect(baseX, baseY - 2 * strideY, hunger * fullWidth, barHeight);

	engone::SetColor(0.1, 0.2, 1, 1);
	engone::DrawRect(baseX, baseY - 3 * strideY, mana * fullWidth, barHeight);

	engone::SetColor(1, 1, 1, 1);
	engone::SetSize(1, 1);

	float textX = x + fullWidth / 2 + barBorderW;
	float textY = y + h - barBorderH - barHeight / 2;

	engone::SetTransform(textX, textY);
	engone::DrawString("consolas42", std::to_string((int)plr->health) + "/" + std::to_string((int)plr->maxHealth), true, barHeight, -1);

	engone::SetTransform(textX, textY - strideY);
	engone::DrawString("consolas42", std::to_string((int)plr->stamina) + "/" + std::to_string((int)plr->maxStamina), true, barHeight, -1);

	engone::SetTransform(textX, textY - 2 * strideY);
	engone::DrawString("consolas42", std::to_string((int)plr->hunger) + "/" + std::to_string((int)plr->maxHunger), true, barHeight, -1);

	engone::SetTransform(textX, textY - 3 * strideY);
	engone::DrawString("consolas42", std::to_string((int)plr->mana) + "/" + std::to_string((int)plr->maxMana), true, barHeight, -1);

}