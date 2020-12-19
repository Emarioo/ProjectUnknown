#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Rendering/BufferContainer.h"

class MeshData {
public:
	MeshData(){}
	
	MaterialType material=MaterialType::ColorMat;
	std::string texture;
	glm::vec3 position=glm::vec3(1);
	glm::vec3 rotation=glm::vec3(1);
	BufferContainer container;
	
	bool hasError = false;
private:

};