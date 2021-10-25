#include "gonpch.h"

#include "Model.h"

#include "../Handlers/AssetManager.h"

namespace engone {
	
	void Model::AddMesh(const std::string& name, glm::mat4 matrix) {
		Mesh* mesh = GetMeshAsset(name);
		if (mesh != nullptr) {
			meshes.push_back(mesh);
			matrices.push_back(matrix);
		} else hasError = true;
	}
	bool Model::AddAnimation(const std::string& name) {
		Animation* anim = GetAnimationAsset(name);
		if (anim != nullptr) {
			animations.push_back(anim);
			return true;
		} else hasError = true;
		return false;
	}
	/*
	void Model::SetArmature(const std::string& name) {
		Armature* arma = GetArmatureAsset(name);
		if (arma != nullptr) {
			armature = arma;
		} else hasError = true;
	}
	*/
	void Model::SetCollider(const std::string& name) {
		colliderName = name;
		hasCollider = true;
	}
}