#pragma once

#include <glm/glm.hpp>
#include "Data/MeshData.h"

class MeshComponent{
public:
	MeshComponent() : mesh(nullptr), matrix(1) {}
	MeshComponent(MeshData* m) : mesh(m), matrix(1) {}
	void SetMesh(MeshData* m);
	MeshData* mesh;
	void SetMatrix(glm::mat4 m);
	glm::mat4 matrix;

	bool hasError=false;

private:

};