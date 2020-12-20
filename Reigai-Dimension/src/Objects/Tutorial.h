#pragma once

#include "GameObject.h"

class Tutorial : public GameObject {
public:
	Tutorial(float x, float y, float z);
	void PreComponents() override;
	void Update(float delta) override;
	std::vector<ColliderComponent*> GetColliders() override;

	RenderComponent parkour;

	ColliderComponent collider;

};