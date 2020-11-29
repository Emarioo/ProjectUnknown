#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Rendering/BufferContainer.h"

class MeshData {
public:
	MeshData(){}
	
	std::string texture;
	glm::vec3 position;
	glm::vec3 rotation;
	MaterialType material;
	BufferContainer container;
	
	bool hasError = false;
private:

};