#include "DataManager.h"

// Dimension
std::map<std::string, Dimension> dimensions;
Dimension* loadedDim = nullptr;
void AddDimension(std::string s, Dimension dim) {
	dimensions[s] = dim;
}
Dimension* GetDimension() {
	return loadedDim;
}
void SetDimension(std::string s) {
	loadedDim = &dimensions[s];
}
// Animation
std::unordered_map<std::string, AnimData*> animations;
void AddAnim(std::string name, std::string path) {
	AnimData* an = new AnimData();
	int err = FHLoadAnim(an, path);
	if (err == FHSuccess) {
		animations[name] = an;
	} else if (err == FHNotFound) {
		std::cout << "[Animation] Not Found: " << path << ".anim" << std::endl;;
		delete an;
	} else if (err == FHCorrupt) {
		std::cout << "[Animation] Corrupt: " << path << ".anim" << std::endl;;
		delete an;
	}
}
AnimData* GetAnim(std::string name) {
	if (animations[name] == nullptr)
		std::cout << "[Animation] '" << name << "' does not exist" << std::endl;

	return animations[name];
}

// Mesh
std::unordered_map<std::string, MeshData*> meshes;
void AddMesh(std::string name, std::string path) {
	MeshData* me = new MeshData();
	int err = FHLoadMesh(me, path);
	if (err == FHSuccess) {
		meshes[name] = me;
	} else if (err == FHNotFound) {
		std::cout << "[MeshData] Not Found: " << path << ".mesh" << std::endl;;
		delete me;
	} else if (err == FHCorrupt) {
		std::cout << "[MeshData] Corrupt: " << path << ".mesh" << std::endl;;
		delete me;
	}
}
MeshData* GetMesh(std::string name) {
	if (meshes[name] == nullptr)
		std::cout << "[MeshData] '" << name << "' does not exist" << std::endl;

	return meshes[name];
}
glm::vec3 GetMeshPos(std::string name) {
	MeshData* me = meshes[name];
	if (me != nullptr)
		return me->position;
	else
		return glm::vec3(0, 0, 0);
}
glm::vec3 GetMeshRot(std::string name) {
	MeshData* me = meshes[name];
	if (me != nullptr)
		return me->rotation;
	else
		return glm::vec3(0, 0, 0);
}

// Collider
std::unordered_map<std::string, CollData*> colliders;
void AddColl(std::string name, std::string path) {
	CollData* da = new CollData();
	int err = FHLoadColl(da, path);
	if (err == FHSuccess) {
		colliders[name] = da;
	} else if (err == FHNotFound) {
		std::cout << "[CollData] Not Found: " << path << ".coll" << std::endl;;
		delete da;
	} else if (err == FHCorrupt) {
		std::cout << "[CollData] Corrupt: " << path << ".coll" << std::endl;;
		delete da;
	}
}
CollData* GetColl(std::string name) {
	if (colliders[name] == nullptr)
		std::cout << "[CollData] '" << name << "' does not exist" << std::endl;

	return colliders[name];
}