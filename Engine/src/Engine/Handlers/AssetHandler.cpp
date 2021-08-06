#include "AssetHandler.h"

#include <Windows.h>


namespace engine {

	std::vector<TimedFunc> functions;
	void AddTimedFunction(std::function<void()> func, float time) {
		functions.push_back(TimedFunc(func,time));
	}
	void UpdateTimedFunc(float delta) {
		int n = 0;
		for (TimedFunc& f : functions) { // Will this cuse problems? Removing an element in a for loop
			f.time -= delta;
			if (f.time < 0) {
				f.func();
				functions.erase(functions.begin() + n);
				n--;
			}
			n++;
		}
	}
	std::unordered_map<std::string, Material> materials;
	void AddMaterialAsset(const std::string& file) {
		if (materials.count(file) == 0) {
			materials[file] = Material();
			LoadMaterial(&materials[file], file);
		}
	}
	Material* GetMaterialAsset(const std::string& name) {
		if (materials.count(name) == 0) {
			bug::out < bug::RED < "Cannot find Material '" < name < "'\n";
		} else if (!materials[name].hasError)
			return &materials[name];
		return nullptr;
	}
	void DeleteMaterialAsset(const std::string& name) {
		materials.erase(name);
	}
	std::unordered_map<std::string, Mesh> meshes;
	void AddMeshAsset(const std::string& file) {
		if (meshes.count(file) == 0) {
			meshes[file] = Mesh();
			LoadMesh(&meshes[file], file);
		}
	}
	Mesh* GetMeshAsset(const std::string& name) {
		if (meshes.count(name) == 0) {
			bug::out < bug::RED < "Cannot find Mesh '" < name < "'\n";
		} else if (!meshes[name].hasError)
			return &meshes[name];
		return nullptr;
	}
	void DeleteMeshAsset(const std::string& name) {
		meshes.erase(name);
	}
	std::unordered_map<std::string, Animation> animations;
	void AddAnimationAsset(const std::string& file) {
		if (animations.count(file) == 0) {
			animations[file] = Animation();
			LoadAnimation(&animations[file], file);
		}
	}
	Animation* GetAnimationAsset(const std::string& name) {
		if (animations.count(name) == 0) {
			bug::out < bug::RED < "Cannot find Animation '" < name < "'\n";
		} else if (!animations[name].hasError)
			return &animations[name];
		return nullptr;
	}
	void DeleteAnimationAsset(const std::string& name) {
		animations.erase(name);
	}
	std::unordered_map<std::string, Armature> armatures;
	void AddArmatureAsset(const std::string& file) {
		if (armatures.count(file) == 0) {
			armatures[file] = Armature();
			LoadArmature(&armatures[file], file);
		}
	}
	Armature* GetArmatureAsset(const std::string& name) {
		if (armatures.count(name) == 0) {
			bug::out < bug::RED < "Cannot find Armature '" < name < "'\n";
		} else if (!armatures[name].hasError)
			return &armatures[name];
		return nullptr;
	}
	void DeleteArmatureAsset(const std::string& name) {
		armatures.erase(name);
	}
	std::unordered_map<std::string, Collider> colliders;
	void AddColliderAsset(const std::string& file) {
		if (colliders.count(file) == 0) {
			colliders[file] = Collider();
			LoadCollider(&colliders[file], file);
		}
	}
	Collider* GetColliderAsset(const std::string& name) {
		if (colliders.count(name) == 0) {
			bug::out < bug::RED < "Cannot find Collider '" < name < "'\n";
		} else if (!colliders[name].hasError)
			return &colliders[name];
		return nullptr;
	}
	void DeleteColliderAsset(const std::string& name) {
		colliders.erase(name);
	}
	std::unordered_map<std::string, Model> models;
	void AddModelAsset(const std::string& file) {
		if (models.count(file) == 0) {
			models[file] = Model();
			LoadModel(&models[file], file);
		}
	}
	Model* GetModelAsset(const std::string& name) {
		if (models.count(name) == 0) {
			AddModelAsset(name);
			// bug::out < bug::RED < "Cannot find Model '" < name < "'\n";
		}
		if (!models[name].hasError)
			return &models[name];
		return nullptr;
	}
	void DeleteModelAsset(const std::string& name) {
		models.erase(name);
	}
	std::unordered_map<std::string, Texture> textures;
	void AddTextureAsset(const std::string& name) {// Change this system so it is simular to every other system. FileExist?
		std::string path = "assets/textures/" + name + ".png";
		if (FileExist(path)) {
			textures[name] = Texture(path);
		} else {
			bug::out < bug::RED < "Cannot find Texture '" < path < bug::end;
		}
	}
	Texture* GetTextureAsset(const std::string& name) {
		if (textures.count(name) == 0) {
			bug::out < bug::RED < "Cannot find Texture '" < name < "'\n";
		} else
			return &textures[name];
		return nullptr;
	}
	void DeleteTextureAsset(const std::string& name) {
		textures.erase(name);
	}
}