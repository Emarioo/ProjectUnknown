#pragma once

#include "IElem.h"

class IInput : public IElem {
public:
	IInput(std::string name, std::string dim, const std::string& texture);
	IInput(std::string name, std::string dim, float r, float g, float b, float a);
	IInput(std::string name, std::string dim, float r, float g, float b, float a, const std::string& texture);

	//void Update(float delta) override;
	//void Draw() override;

	void Type(int key, int action);// key?
	void Typing();
};