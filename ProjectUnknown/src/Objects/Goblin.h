#pragma once

#include "Engone/Objects/GameObject.h"

class Goblin : public engone::GameObject {
private:
	float blending = 0;
	float speed = 1.7;

public:
	Goblin(float x, float y, float z);
	//void Update(float delta) override;

};