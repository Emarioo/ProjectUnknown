#pragma once

#include "GameObject.h"

class Goblin : public GameObject {
public:
	Goblin(float x, float y, float z);
	void PreComponents() override;
	void Update(float delta) override;

	MeshComponent mesh;
	BoneComponent bone;
	AnimationComponent slash;

};