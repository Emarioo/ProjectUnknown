#include "DataManager.h"

#include <Windows.h>


namespace engine {

	std::vector<TimedFunc> functions;
	void AddTimedFunction(std::function<void()> func, float time) {
		functions.push_back(TimedFunc(func,time));
	}
	void UpdateTimedFunc(float delta) {
		int n = 0;
		for (TimedFunc& f : functions) {
			f.time -= delta;
			if (f.time < 0) {
				f.func();
				functions.erase(functions.begin() + n);
				n--;
			}
			n++;
		}
	}
	
	std::unordered_map<std::string, AnimData> animations;
	AnimData* AddAnimAsset(const std::string& file) {
		animations[file] = AnimData();
		int err = LoadAnim(&animations[file], file);
		if (err != FileReport::Success)
			animations[file].hasError = true;
		return &animations[file];
	}
	AnimData* GetAnimAsset(const std::string& name) {
		if (animations.count(name) == 0) {
			bug::out < bug::RED < "Cannot find AnimData '" < name < "'\n";
		} else if (!animations[name].hasError)
			return &animations[name];
		return nullptr;
	}
	
	std::unordered_map<std::string, BoneData> bones;
	void AddBoneAsset(const std::string& file) {
		bones[file] = BoneData();
		int err = LoadBone(&bones[file], file);
		if (err != FileReport::Success)
			bones[file].hasError = true;
	}
	BoneData* GetBoneAsset(const std::string& name) {
		if (bones.count(name) == 0) {
			bug::out < bug::RED < "Cannot find BoneData '" < name < "'\n";
		} else if (!bones[name].hasError)
			return &bones[name];
		return nullptr;
	}
	
	std::unordered_map<std::string, MeshData> meshes;
	void AddMeshAsset(const std::string& file) {
		meshes[file] = MeshData();
		int err = LoadMesh(&meshes[file], file);
		if (err != FileReport::Success)
			meshes[file].hasError = true;
	}
	MeshData* GetMeshAsset(const std::string& name) {
		if (meshes.count(name) == 0) {
			bug::out < bug::RED < "Cannot find MeshData '" < name < "'\n";
		} else if (!meshes[name].hasError)
			return &meshes[name];
		return nullptr;
	}
	/*
	glm::vec3 GetMeshPos(const std::string& name) {
		if (meshes.count(name) > 0)
			return meshes[name].position;
		return glm::vec3(0, 0, 0);
	}
	glm::vec3 GetMeshRot(const std::string& name) {
		if (meshes.count(name) > 0)
			return meshes[name].rotation;
		return glm::vec3(0, 0, 0);
	}
	*/
	std::unordered_map<std::string, CollData> colliders;
	void AddCollAsset(const std::string& file) {
		colliders[file] = CollData();
		int err = LoadColl(&colliders[file], file);
		if (err != FileReport::Success)
			meshes[file].hasError = true;
	}
	/*CollData* AddColl(const std::string& name) {
		colliders[name] = CollData();
		return &colliders[name];
	}*/
	CollData* GetCollAsset(const std::string& name) {
		if (colliders.count(name) == 0) {
			bug::out < bug::RED < "Cannot find CollData '" < name < "'\n";
		} else if (!colliders[name].hasError)
			return &colliders[name];
		return nullptr;
	}
}