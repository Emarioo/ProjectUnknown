#pragma once

#include "GameObject.h"

class MagicStaff : public GameObject {
public:
	MagicStaff(float x,float y,float z);
	void Draw() override;
	void Update(float delta) override;

	Animation* idle;

};