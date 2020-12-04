#include "MeshComponent.h"

void MeshComponent::AddMesh(MeshData* mesh) {
	meshes.push_back(mesh);
	matrices.push_back(glm::mat4(1));
}
void MeshComponent::SetBone(BoneComponent* comp) {
	bone = comp;
}