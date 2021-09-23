#pragma once

#include "../Handlers/FileHandler.h"
#include "../Rendering/Texture.h"
#include "../DebugTool/DebugHandler.h"


namespace engone {
	
	struct TimedFunc {
		TimedFunc(std::function<void()> f, float t) : func(f), time(t) {}
		std::function<void()> func;
		float time = 0;
	};

	void AddTimedFunction(std::function<void()> func,float time);
	void UpdateTimedFunc(float delta);

	/*
	If file isn't found an error will occur and an empty object will
	be created. Set the object's hasError to false to modify
	*/
	void AddAnimationAsset(const std::string& file);
	/*
	returns nullptr if name doesn't exist
	*/
	Animation* GetAnimationAsset(const std::string& name);
	void DeleteAnimationAsset(const std::string& name);

	void AddArmatureAsset(const std::string& file);
	/*
	returns nullptr if name doesn't exist
	*/
	Armature* GetArmatureAsset(const std::string& name);

	void DeleteArmatureAsset(const std::string& name);

	void AddMaterialAsset(const std::string& file);
	Material* GetMaterialAsset(const std::string& name);
	void DeleteMaterialAsset(const std::string& name);

	void AddMeshAsset(const std::string& file);
	Mesh* GetMeshAsset(const std::string& name);
	void DeleteMeshAsset(const std::string& name);

	void AddColliderAsset(const std::string& file);
	/*
	Add empty collider. Used for objects with unique collisions
	*/
	//CollData* AddColl(const std::string& name);
	/*
	Will return nullptr if collider doesn't exist
	*/
	Collider* GetColliderAsset(const std::string& name);
	void DeleteColliderAsset(const std::string& name);

	void AddModelAsset(const std::string& file);
	/*
	Modified to try adding model asset to the game unless it already is loaded.
	*/
	Model* GetModelAsset(const std::string& name);
	void DeleteModelAsset(const std::string& name);

	void AddTextureAsset(const std::string& name);
	Texture* GetTextureAsset(const std::string& name);
	void DeleteTextureAsset(const std::string& name);
}