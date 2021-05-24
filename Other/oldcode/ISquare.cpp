#include "ISquare.h"

ISquare::ISquare(const std::string& name, const std::string& dim4, const std::string& texture){
	this->name = name;
	float arr[4];
	DimFormat(dim4,arr,4);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];

	this->texture = texture;
	UpdateCont();
}
ISquare::ISquare(const std::string& name, const std::string& dim4, float r, float g, float b, float a) {
	this->name = name;
	float arr[4];
	DimFormat(dim4, arr,4);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];
	this->r = r; this->g = g; this->b = b; this->a = a;
	
	UpdateCont();
}
ISquare::ISquare(const std::string& name, const std::string& dim4, float r, float g, float b, float a, const std::string& texture) {
	this->name = name;
	float arr[4];
	DimFormat(dim4, arr,4);
	x = arr[0]; y = arr[1], w = arr[2], h = arr[3];
	this->r = r; this->g = g; this->b = b; this->a = a;
	this->texture = texture;
	UpdateCont();
}/*
void ISquare::Draw() {
	if (!HasTags()) return;
	DrawNormal();
}*/