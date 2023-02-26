#include "IList.h"

IList::IList(const std::string& name,Font* f, const std::string& dim, const std::string& texture1, const std::string& texture2, std::function<void(int)> func, float time) {
	this->name = name;
	font = f;
	float arr[7];
	DimFormat(dim, arr, 7);
	x = arr[0]; y = arr[1]; w = arr[2]; h = arr[3]; spacing = arr[4]; innerHeight = arr[5]; charHeight = arr[6];
	texture = texture1;
	this->texture2 = texture2;
	Click.Func(func, time);
	//UpdateCont();
}
IList::IList(const std::string& name, Font* f, const std::string& dim, float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2, std::function<void(int)> func, float time) {
	this->name = name;
	font = f;
	float arr[7];
	DimFormat(dim, arr, 7);
	x = arr[0]; y = arr[1]; w = arr[2]; h = arr[3]; spacing = arr[4]; innerHeight = arr[5]; charHeight = arr[6];
	r = r1;
	g = g1;
	b = b1;
	a = a1;
	this->r2 = r2; this->g2 = g2; this->b2 = b2; this->a2 = a2;
	Click.Func(func, time);
	//UpdateCont();
}

std::vector<std::string> items;
void IList::AddItem(const std::string& item) {
	items.push_back(item);
}

// name w spacing iw ih colorB colorI textureB textureI
//    dim color texture text

void IList::UpdateCont() {
	//text.ElemWH(GetW(), GetH());
}

bool IList::ClickEvent(int mX, int mY, int action) {
	if (!HasTags()) return false;

	if (action != 1) return false;

	float w = GetW() - 2 * spacing;
	float h = innerHeight;

	float x = GetX();
	float gy = GetY();

	float mx = renderer::AlterX(mX);
	float my = renderer::AlterY(mY);

	clicked = false;
	selected = false;
	//Click.ResetFunc();
	for (int i = 0; i < items.size(); i++) {
		float y = gy + i * (innerHeight + spacing);
		bug::outs < mx < my < x < y < w < h < bug::end;
		if (mx > x - w / 2 && mx<x + w / 2 && my>y - h / 2 && my < y + h / 2) {
			funcData = i;
			clicked = action == 1;
			selected = true;
			break;
		}
	}
	return clicked;

	if (Inside(mx, my)) {
		clicked = action == 1;
		selected = true;
	} else {
		clicked = false;
		selected = false;
	}
	return clicked;
}

void IList::Draw() {
	if (!HasTags()) return;
	DrawNormal();

	float x = GetX();
	float y = GetY();

	// Draw items
	renderer::GuiColor(r2,g2,b2,a2);
	renderer::BindTexture(texture2);
	renderer::GuiSize(GetW()-2*spacing,innerHeight);
	for (int i = 0; i < items.size();i++) {
		renderer::DrawRect(x, y + i * (innerHeight + spacing));
	}

	if (font != nullptr) {
		if (font->texture != nullptr)
			font->texture->Bind();
	}
	renderer::GuiColor(1, 1, 1, 1);
	renderer::GuiSize(1, 1);
	for (int i = 0; i < items.size(); i++) {
		std::string& s = items[i];
		renderer::GuiTransform(x, y + i * (innerHeight + spacing));
		renderer::DrawText(font,s,true,charHeight,-1);
	}
	/*
	// Draw Scroll Edges
	renderer::GuiColor(r,g,b,a);
	renderer::GuiSize(GetW() - 2 * spacing, innerHeight);
	renderer::DrawRect(x, y -h/2);
	renderer::DrawRect(x, y +h/2);
	*/
}