#pragma once

#include "Engine/GameObject.h"

class MagicStaff : public engine::GameObject {
public:
	MagicStaff(float x,float y,float z);
	void PreComponents() override;
	void Update(float delta) override;
	std::vector<engine::ColliderComponent*> GetColliders() override;

	//RenderComponent mesh;

	//AnimationComponent idle;

};