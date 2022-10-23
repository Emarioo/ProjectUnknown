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
	engone::GetTexture("infobar")->Bind();

	engone::Shader* gui = engone::GetShader("gui");
	gui->SetVec2("uPos", { x,y });
	gui->SetVec2("uSize", { w,h });
	gui->SetVec4("uColor", color.r, color.g, color.b, color.a);
	gui->SetInt("uTextured", 1);

	engone::DrawRect();

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

	gui->SetInt("uTextured", 0);

	float baseX = x + barBorderW;
	float baseY = y + h - barBorderH - barHeight;
	float strideY = (barHeight + barGap);

	gui->SetVec2("uPos", { baseX, baseY });
	gui->SetVec2("uSize", { health * fullWidth, barHeight });
	gui->SetVec4("uColor", 0, 1, 0, 1);
	engone::DrawRect();

	gui->SetVec2("uPos", { baseX, baseY - strideY });
	gui->SetVec2("uSize", { stamina * fullWidth, barHeight });
	gui->SetVec4("uColor", 1, 1, 0, 1);
	engone::DrawRect();

	gui->SetVec2("uPos", { baseX, baseY - 2 * strideY });
	gui->SetVec2("uSize", { hunger * fullWidth, barHeight });
	gui->SetVec4("uColor", 0.8, 0.5, 0.2, 1);
	engone::DrawRect();

	gui->SetVec2("uPos", { baseX, baseY - 3 * strideY });
	gui->SetVec2("uSize", { mana * fullWidth, barHeight });
	gui->SetVec4("uColor", 0.1, 0.2, 1, 1);
	engone::DrawRect();

	gui->SetVec2("uSize", { 1,1 });
	gui->SetVec4("uColor", 1, 1, 1, 1);

	float textX = x + fullWidth / 2 + barBorderW;
	float textY = y + h - barBorderH - barHeight / 2;

	engone::Font* consolas = engone::GetFont("consolas");

	gui->SetVec2("uPos", { textX,textY });
	engone::DrawString(consolas, std::to_string((int)plr->health) + "/" + std::to_string((int)plr->maxHealth), true, barHeight,fullWidth,barHeight);

	gui->SetVec2("uPos", { textX,textY-strideY });
	engone::DrawString(consolas, std::to_string((int)plr->stamina) + "/" + std::to_string((int)plr->maxStamina), true, barHeight, fullWidth, barHeight);

	gui->SetVec2("uPos", { textX,textY - strideY*2 });
	engone::DrawString(consolas, std::to_string((int)plr->hunger) + "/" + std::to_string((int)plr->maxHunger), true, barHeight, fullWidth, barHeight);

	gui->SetVec2("uPos", { textX,textY - strideY*3 });
	engone::DrawString(consolas, std::to_string((int)plr->mana) + "/" + std::to_string((int)plr->maxMana), true, barHeight, fullWidth, barHeight);

}