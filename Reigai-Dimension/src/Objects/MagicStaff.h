#pragma once

#include "GameObject.h"

class MagicStaff : public GameObject {
public:
	MagicStaff(float x,float y,float z);
	void PreComponents() override;
	void Update(float delta) override;
	std::vector<ColliderComponent*> GetColliders() override;

	MeshComponent staff;
	MeshComponent fireball;

	//AnimationComponent idle;

};