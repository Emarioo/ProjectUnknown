#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Rendering/BufferContainer.h"


class Mesh
{
public:
	Mesh(){}
	std::string tex;
	glm::vec3 position;
	glm::vec3 rotation;
	BufferContainer container;
private:

};