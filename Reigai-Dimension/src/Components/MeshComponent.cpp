#include "MeshComponent.h"

void MeshComponent::SetMesh(MeshData* m) {
	mesh = m;
}
void MeshComponent::SetMatrix(glm::mat4 m) {
	matrix = m;
}