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

		float barHeight = h * (48/226.f);
		float barStride = h * (58/226.f);
		float fullWidth = w * (546/550.f);
		float barBorderW = w * (2 / 550.f);
		float barRestH = barStride-barHeight;
		float barBorderH = h * (2 / 226.f);

		engine::BindTexture(0, "blank");
		engine::GuiColor(0, 1, 0, 1);
		engine::DrawRect(x-w/2+ fullWidth/2 + barBorderW + (health-1)*fullWidth/2,y+h/2 - (barStride - barRestH)/2 - barBorderH,health * fullWidth,barHeight);
		
		engine::GuiColor(1, 1, 0, 1);
		engine::DrawRect(x-w/2+ fullWidth/2 + barBorderW + (stamina-1)*fullWidth/2,y+h/2 - (3*barStride - barRestH)/2 - barBorderH,stamina * fullWidth,barHeight);
		
		engine::GuiColor(0.8, 0.5, 0.2, 1);
		engine::DrawRect(x-w/2+ fullWidth/2 + barBorderW + (hunger-1)*fullWidth/2,y+h/2 - (5*barStride - barRestH) /2 - barBorderH,hunger * fullWidth,barHeight);
		
		engine::GuiColor(0.1, 0.2, 1, 1);
		engine::DrawRect(x-w/2+ fullWidth/2 + barBorderW + (mana-1)*fullWidth/2,y+h/2 - (7*barStride - barRestH) /2 - barBorderH,mana * fullWidth,barHeight);
		
		engine::GuiColor(1, 1, 1, 1);
		engine::GuiSize(1, 1);

		engine::GuiTransform(x - w / 2 + fullWidth / 2, y + h / 2 - (barStride - barRestH) / 2 - barBorderH);
		engine::DrawString(engine::GetFont(), std::to_string((int)plr->health) + "/" + std::to_string((int)plr->maxHealth), true, barHeight, -1);
	
		engine::GuiTransform(x - w / 2 + fullWidth / 2, y + h / 2 - (3*barStride - barRestH) / 2 - barBorderH);
		engine::DrawString(engine::GetFont(), std::to_string((int)plr->stamina) + "/" + std::to_string((int)plr->maxStamina), true, barHeight, -1);
		
		engine::GuiTransform(x - w / 2 + fullWidth / 2, y + h / 2 - (5*barStride - barRestH) / 2 - barBorderH);
		engine::DrawString(engine::GetFont(), std::to_string((int)plr->hunger) + "/" + std::to_string((int)plr->maxHunger), true, barHeight, -1);
		
		engine::GuiTransform(x - w / 2 + fullWidth / 2, y + h / 2 - (7*barStride - barRestH) / 2 - barBorderH);
		engine::DrawString(engine::GetFont(), std::to_string((int)plr->mana) + "/" + std::to_string((int)plr->maxMana), true, barHeight, -1);
	}
}