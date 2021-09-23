#pragma once

#include "Engone/Objects/GameObject.h"

class Gnorg : public engone::GameObject {
public:
	Gnorg(float x, float y, float z);
	void Update(float delta) override;

};