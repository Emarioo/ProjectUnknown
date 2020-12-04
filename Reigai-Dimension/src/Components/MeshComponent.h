#pragma once

#include <glm/glm.hpp>
#include "Data/MeshData.h"
#include "Components/BoneComponent.h"

class MeshComponent{
public:
	MeshComponent() {}
	
	std::vector<MeshData*> meshes;
	std::vector<glm::mat4> matrices;
	void AddMesh(MeshData* mesh);

	BoneComponent* bone = nullptr;
	void SetBone(BoneComponent* comp);

	bool hasError=false;

private:

};