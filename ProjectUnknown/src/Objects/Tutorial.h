#pragma once

#include "Engone/Objects/GameObject.h"

class Tutorial : public engone::GameObject {
public:
	Tutorial(float x, float y, float z);
	void Update(float delta) override;

};