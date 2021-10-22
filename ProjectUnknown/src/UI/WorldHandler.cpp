#include "propch.h"

#include "InterfaceManager.h"

//#include "Keybindings.h"

WorldItem::WorldItem(const std::string& name)
: name(name){
	
}

WorldHandler::WorldHandler(const std::string& name) : engone::IBase(name) {
	active = true;
	worldItems.push_back(WorldItem("World 1"));
	worldItems.push_back(WorldItem("Duttot bepop"));
	worldItems.push_back(WorldItem("World 2"));
}
bool WorldHandler::MouseEvent(int mx, int my, int action, int button) {
	float mouseX = engone::ToFloatScreenX(mx);
	float mouseY = engone::ToFloatScreenY(my);

	if (action != 1)
		return false;

	return false;
}
bool WorldHandler::KeyEvent(int key, int action) {
	
	return false;
}
bool WorldHandler::ScrollEvent(double scroll) {
	float mouseX = engone::GetFloatMouseX();
	float mouseY = engone::GetFloatMouseY();
	
	if (x<mouseX&&mouseX<x+w&&y<mouseY&&mouseY<y+h) {
		scrollOffset -= scroll / 18;
	}
	return false;
}
void WorldHandler::Update(float delta) {

}
void WorldHandler::Render() {

	using namespace engone;
	Shader* gui = GetShader("gui");

	// background
	if (color.a != 0) {
		gui->SetInt("uTextured", 0);
		engone::DrawRect(gui,x, y, w, h, color.r, color.g, color.b, color.a);
	}

	float itemStride = h / 8;
	float itemGap = itemStride/10;

	for (int i = 0; i < worldItems.size();i++) {
		WorldItem& item = worldItems[i];
		gui->SetInt("uTextured", 0);
		//bug::out < y < bug::end;
		engone::DrawRect(gui,x+itemGap, y+h-itemStride*(i+1)+scrollOffset, w-2*itemGap, itemStride-itemGap, 0.4f, 0.2f, 0.6f, 1);

		gui->SetVec2("uPos", { x + itemGap + (w - 2 * itemGap) / 2,y + h - itemStride * (i + 1) + (itemStride - itemGap) / 2 + scrollOffset });
		gui->SetVec2("uSize", { 1,1 });
		gui->SetVec4("uColor", 1,1,1,1 );
		gui->SetInt("uTextured", 1);

		engone::DrawString(GetFont("consolas42"), item.name,true, itemStride-itemGap,w - 2 * itemGap,itemStride - itemGap);
	}
}