#pragma once

#include "IElem.h"
#include <functional>

class IButton : public IElem {
public:
	IButton(const std::string& name, const std::string& dim4, const std::string& texture,std::function<void(int)> func, float time);
	IButton(const std::string& name, const std::string& dim4, float r, float g, float b, float a, std::function<void(int)> func, float time);
	IButton(const std::string& name, const std::string& dim4, float r, float g, float b, float a, const std::string& texture, std::function<void(int)> func, float time);

	//void Draw() override;

};