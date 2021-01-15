#pragma once

#include "Engine/GameObject.h"

class Tutorial : public engine::GameObject {
public:
	Tutorial(float x, float y, float z);
	void PreComponents() override;
	void Update(float delta) override;

};