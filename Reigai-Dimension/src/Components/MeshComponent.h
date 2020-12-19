#pragma once

#include <glm/glm.hpp>
#include "Data/MeshData.h"
#include "Components/BoneComponent.h"

class MeshComponent{
public:
	MeshComponent() {}
	
	BoneComponent* bone=nullptr;
	void SetBone(BoneComponent* comp);

	std::vector<MeshData*> meshes;
	std::vector<glm::mat4> matrices;
	void AddMesh(MeshData* mesh);
	void SetMatrix(int index, glm::mat4 m);

	bool hasError=false;

private:

};