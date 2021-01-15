#include "RenderComponent.h"

namespace engine {

	RenderComponent::RenderComponent() {
		bone.SetAnim(&anim);
		bone.SetAnim(&anim);
	}
	void RenderComponent::AddMesh(const std::string& data) {
		MeshData* mesh = GetMeshAsset(data);
		if (mesh != nullptr) {
			enabled = true;
			meshes.push_back(mesh);
			matrices.push_back(glm::mat4(1));
		}
	}
	void RenderComponent::SetMatrix(int index, glm::mat4 m) {
		if (matrices.size() > index)
			matrices[index] = m;
	}
	void RenderComponent::SetBone(const std::string& data) {
		bone.SetBone(GetBoneAsset(data));
	}
	void RenderComponent::SetAnim(const std::string& data) {
		anim.SetAnim(GetAnimAsset(data));
	}
}