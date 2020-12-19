#include "MeshComponent.h"

void MeshComponent::AddMesh(MeshData* mesh) {
	meshes.push_back(mesh);
	matrices.push_back(glm::mat4(1));
}
void MeshComponent::SetMatrix(int index, glm::mat4 m) {
	matrices[index] = m;
}
void MeshComponent::SetBone(BoneComponent* comp) {
	bone = comp;
}
