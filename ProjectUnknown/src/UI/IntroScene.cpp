#include "IntroScene.h"

#include "InterfaceManager.h"

IntroScene::IntroScene(const std::string& name) : engine::IBase(name,999) {
	active = true;
}
float introScene_time=0;
float introScene_alpha1 = 1;
float introScene_fade1 = 0;
float introScene_alpha2 = 0;
float introScene_fade2 = 1;
void IntroScene::Update(float delta) {
	bug::out < delta < bug::end;
	introScene_time += delta;
	if (introScene_time>1&&introScene_time<2) {
		introScene_fade1 = (introScene_time - 1);
	}
	else if (introScene_time > 2 && introScene_time < 3) {
		introScene_alpha2 = (introScene_time - 2);
	} else if (introScene_time > 5 && introScene_time < 6) { // alpha2 does not go to 0 because delta skips over it
		introScene_alpha2 = (1-(introScene_time-5));
	}
	else if (introScene_time > 6 && introScene_time < 7) {
		introScene_fade1 = (1 - (introScene_time - 6));
	}
	else if (introScene_time > 5) {
		active = false;
	}
}
void IntroScene::Render() {
	// Inventory background
	engine::BindTexture(0, "blank");
	engine::DrawRect(-1.f, -1.f, 2.f, 2.f, introScene_fade1, introScene_fade1, introScene_fade1, introScene_alpha1);

	// Logo/Text
	engine::BindTexture(0, "intro");
	engine::DrawRect(x, y, w, h, introScene_fade2, introScene_fade2, introScene_fade2, introScene_alpha2);
}