#pragma once

#include "Engine/GameObject.h"

class Goblin : public engine::GameObject {
public:
	Goblin(float x, float y, float z);
	void Update(float delta) override;

};