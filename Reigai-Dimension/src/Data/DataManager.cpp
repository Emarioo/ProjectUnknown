#include "DataManager.h"


#include <Windows.h>

namespace dManager {
	Menu currentMenu = Startup;
	Menu GetMenu() {
		return currentMenu;
	}
	void SetMenu(Menu m) {
		currentMenu = m;
	}/*
	ClientPlayer* player;
	ClientPlayer* GetPlayer() {
		return player;
	}*/
	// Dimension
	std::unordered_map<std::string, Dimension> dimensions;
	Dimension* loadedDim = nullptr;
	void AddDimension(const std::string& s, Dimension dim) {
		dimensions[s] = dim;
	}
	Dimension* GetDimension() {
		return loadedDim;
	}
	void SetDimension(const std::string& s) {
		loadedDim = &dimensions[s];
	}
	// Animation
	std::unordered_map<std::string, AnimData> animations;
	void AddAnim(const std::string& name, const std::string& path) {
		animations[name] = AnimData();
		int err = fManager::LoadAnim(&animations[name], path);
		if (err != fManager::Success)
			animations[name].hasError = true;
	}
	AnimData* GetAnim(const std::string& name) {
		if (animations.count(name) == 0) {
			bug::out + bug::RED + "Cannot find AnimData '" + name + "'\n";
		} else if (!animations[name].hasError)
			return &animations[name];
		return nullptr;
	}
	// Mesh
	std::unordered_map<std::string, MeshData> meshes;
	void AddMesh(const std::string& name, const std::string& path) {
		meshes[name] = MeshData();
		int err = fManager::LoadMesh(&meshes[name], path);
		if (err != fManager::Success)
			meshes[name].hasError = true;
	}
	MeshData* GetMesh(const std::string& name) {
		if (meshes.count(name) == 0) {
			bug::out + bug::RED + "Cannot find MeshData '" + name + "'\n";
		} else if (!meshes[name].hasError)
			return &meshes[name];
		return nullptr;
	}
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
	// Collider
	std::unordered_map<std::string, CollData> colliders;
	void AddColl(const std::string& name, const std::string& path) {
		colliders[name] = CollData();
		int err = fManager::LoadColl(&colliders[name], path);
		if (err != fManager::Success)
			meshes[name].hasError = true;
	}
	CollData* AddColl(const std::string& name) {
		colliders[name] = CollData();
		return &colliders[name];
	}
	CollData* GetColl(const std::string& name) {
		if (colliders.count(name) == 0) {
			bug::out + bug::RED + "Cannot find CollData '" + name + "'\n";
		} else if (!colliders[name].hasError)
			return &colliders[name];
		return nullptr;
	}
	std::vector<MeshComponent*> meshComponents;
	std::vector<ColliderComponent*> collComponents;
	void AddComponent(MeshComponent* comp) {
		meshComponents.push_back(comp);
	}
	void AddComponent(ColliderComponent* comp) {
		collComponents.push_back(comp);
	}
	std::vector<MeshComponent*>* GetMeshComponents() {
		return &meshComponents;
	}
	std::vector<ColliderComponent*>* GetColliderComponents() {
		return &collComponents;
	}
	void Init() {

	}
}