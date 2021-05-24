#pragma once

#include "IElem.h"

class ISquare : public IElem {
public:
	ISquare(const std::string& name, const std::string& dim4, const std::string& texture);
	ISquare(const std::string& name, const std::string& dim4, float r,float g,float b,float a);
	ISquare(const std::string& name, const std::string& dim4, float r,float g,float b,float a,const const std::string& texture);

	//void Draw() override;
	
};