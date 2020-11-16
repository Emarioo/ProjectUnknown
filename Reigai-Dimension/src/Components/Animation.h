#pragma once

#include "Rendering/Renderer.h"
#include "Data/AnimData.h"

class Animation {
public:
	Animation(std::string name);
	AnimData* data;
	float frame=0;
	float fps=60;
	bool running=false;
	int GetFrame();
	void Update(float delta);
	glm::vec3 GetPos(std::string o);
	glm::vec3 GetRot(std::string o);
	glm::vec3 GetScale(std::string o);
};