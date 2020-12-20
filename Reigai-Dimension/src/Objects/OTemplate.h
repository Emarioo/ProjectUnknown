#pragma once

#include "GameObject.h"

class OTemplate : public GameObject {
public:
	OTemplate(float x, float y, float z);
	void PreComponents() override;
	void Update(float delta) override;
	std::vector<ColliderComponent*> GetColliders() override;

	RenderComponent mesh;

	ColliderComponent coll;

	//AnimationComponent anim;

};