#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Rendering/BufferContainer.h"

class MeshData {
public:
	MeshData(){}
	std::string name;
	std::string tex;
	glm::vec3 position;
	glm::vec3 rotation;
	BufferContainer container;
	
private:

};