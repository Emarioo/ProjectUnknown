#pragma once

#include "IElem.h"

class IList : public IElem {
public:

	float spacing,innerHeight,charHeight;
	std::string texture2="blank";
	float r2=1, g2=1, b2=1, a2=1;
	Font* font;

	IList(const std::string& name, Font* f, const std::string& dim6, const std::string& texture1,const std::string& texture2, std::function<void(int)> func, float time);
	IList(const std::string& name, Font* f, const std::string& dim6, float r1, float g1, float b1, float a1,float r2,float g2,float b2,float a2, std::function<void(int)> func, float time);

	std::vector<std::string> items;
	void AddItem(const std::string& item);

	// name w spacing ih colorB colorI textureB textureI
	//    dim color texture text

	void UpdateCont() override;

	bool ClickEvent(int mx, int my, int action) override;

	void Draw() override;
};