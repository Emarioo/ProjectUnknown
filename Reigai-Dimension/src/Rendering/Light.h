#pragma once

#include "glm/glm.hpp"

class Light {
public:
	Light(float,float,float);
	Light(float,float,float,float);
	Light(float,float,float,float,float,float,float);
	Light(float,float,float,float,float,float,float,float,float,float);
	glm::vec3 position;
	float range;
	glm::vec3 diffuse;
	glm::vec3 attune;
};