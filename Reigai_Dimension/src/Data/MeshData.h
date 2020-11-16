#pragma once

#include <string>
#include "Rendering/BufferContainer.h"

class MeshData {
public:
	MeshData(){}

	std::string name;
	BufferContainer container;
	
private:
};