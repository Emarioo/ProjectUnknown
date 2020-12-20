#include "RenderComponent.h"

void RenderComponent::AddMesh(MeshData* mesh) {
	meshes.push_back(mesh);
	matrices.push_back(glm::mat4(1));
}
void RenderComponent::SetMatrix(int index, glm::mat4 m) {
	matrices[index] = m;
}
void RenderComponent::SetBone(BoneComponent* comp) {
	bone = comp;
}
