#pragma once

#include "Engine/GameObject.h"

class Gnorg : public engine::GameObject {
public:
	Gnorg(float x, float y, float z);
	void Update(float delta) override;

};