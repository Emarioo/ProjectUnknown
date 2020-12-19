#pragma once

#include "Data/AnimData.h"
#include <glm/glm.hpp>

class AnimationComponent {
public:
	AnimationComponent() {}
	void SetData(AnimData* data);

	AnimData* data=nullptr;
	float frame=0;
	float speed=24; // per second
	bool running=false;
	int GetFrame();
	void Update(float delta);
	/*
	Can cause index out of range
	*/
	glm::mat4 GetTransform(std::string name);
};