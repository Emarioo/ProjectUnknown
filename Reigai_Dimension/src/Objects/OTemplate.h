#pragma once

#include "GameObject.h"

class OTemplate : public GameObject {
public:
	OTemplate(float x, float y, float z);
	void Draw() override;
	void Update() override;

	Animation* ANIM_NAME;
};