#pragma once

#include "IElem.h"

class IInput : public IElem {
public:
	IInput(const std::string& name, const std::string& dim4, const std::string& texture);
	IInput(const std::string& name, const std::string& dim4, float r, float g, float b, float a);
	IInput(const std::string& name, const std::string& dim4, float r, float g, float b, float a, const std::string& texture);

	//void Update(float delta) override;
	//void Draw() override;

	void Type(int key, int action);// key?
	void Typing();
};