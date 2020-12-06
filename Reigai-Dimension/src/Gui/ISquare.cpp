#include "ISquare.h"

ISquare::ISquare(std::string name, std::string dim, const std::string& texture){
	this->name = name;
	float arr[4];
	DimFormat(dim,arr);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];
	this->texture = texture;
	UpdateCont();
}
ISquare::ISquare(std::string name, std::string dim, float r, float g, float b, float a) {
	this->name = name;
	float arr[4];
	DimFormat(dim, arr);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];
	this->r = r; this->g = g; this->b = b; this->a = a;
	UpdateCont();
}
ISquare::ISquare(std::string name, std::string dim, float r, float g, float b, float a, const std::string& texture) {
	this->name = name;
	float arr[4];
	DimFormat(dim, arr);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];
	this->r = r; this->g = g; this->b = b; this->a = a;
	this->texture = texture;
	UpdateCont();
}/*
void ISquare::Draw() {
	if (!HasTags()) return;
	DrawNormal();
}*/