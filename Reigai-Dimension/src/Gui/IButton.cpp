#include "IButton.h"

IButton::IButton(const std::string& name, const std::string& dim, const std::string& texture, std::function<void()> func, float time) {
	this->name = name;
	float arr[4];
	DimFormat(dim, arr);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];
	this->texture = texture;
	UpdateCont();
	Click.Func(func,time);
}
IButton::IButton(const std::string& name, const std::string& dim, float r, float g, float b, float a, std::function<void()> func, float time) {
	this->name = name;
	float arr[4];
	DimFormat(dim, arr);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];
	this->r = r; this->g = g; this->b = b; this->a = a;
	UpdateCont();
	Click.Func(func, time);
}
IButton::IButton(const std::string& name, const std::string& dim, float r, float g, float b, float a, const std::string& texture, std::function<void()> func, float time) {
	this->name = name;
	float arr[4];
	DimFormat(dim, arr);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];
	this->r = r; this->g = g; this->b = b; this->a = a;
	this->texture = texture;
	UpdateCont();
	Click.Func(func, time);
}
/*
void IButton::Draw() {
	if (!HasTags()) return;
	DrawNormal();
}
*/