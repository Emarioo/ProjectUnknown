#pragma once

#include "Data/AnimData.h"
#include <glm/glm.hpp>

class AnimationComponent {
public:
	AnimationComponent() {}
	AnimationComponent(AnimData* anim);
	void SetAnim(AnimData* anim);

	AnimData* data=nullptr;
	float frame=0;
	float fps=60;
	bool running=false;
	int GetFrame();
	void Update(float delta);
	glm::vec3 GetPos(std::string o);
	glm::vec3 GetRot(std::string o);
	glm::vec3 GetScale(std::string o);
};