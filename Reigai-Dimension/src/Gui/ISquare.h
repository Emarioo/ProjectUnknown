#pragma once

#include "IElem.h"

class ISquare : public IElem {
public:
	ISquare(std::string name, std::string dim, const std::string& texture);
	ISquare(std::string name, std::string dim, float r,float g,float b,float a);
	ISquare(std::string name, std::string dim, float r,float g,float b,float a,const std::string& texture);

	//void Draw() override;
	
};