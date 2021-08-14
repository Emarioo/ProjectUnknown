#pragma once

#include "Engone/Objects/GameObject.h"

class MagicStaff : public engine::GameObject {
public:
	MagicStaff(float x,float y,float z);
	void Update(float delta) override;

	//RenderComponent mesh;

	//AnimationComponent idle;

};