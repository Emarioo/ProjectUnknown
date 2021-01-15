#pragma once

#include <unordered_map>
#include "World/Dimension.h"
#include "Managers/FileManager.h"
#include <vector>

#include "Utility/Debugger.h"
#include <functional>


namespace engine {
	
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
	AnimData* AddAnimAsset(const std::string& file);
	/*
	returns nullptr if name doesn't exist
	*/
	AnimData* GetAnimAsset(const std::string& name);

	void AddBoneAsset(const std::string& file);
	/*
	returns nullptr if name doesn't exist
	*/
	BoneData* GetBoneAsset(const std::string& name);

	void AddMeshAsset(const std::string& file);
	MeshData* GetMeshAsset(const std::string& name);
	/*
	glm::vec3 GetMeshPos(const std::string& name);
	glm::vec3 GetMeshRot(const std::string& name);
	*/
	void AddCollAsset(const std::string& file);
	/*
	Add empty collider. Used for objects with unique collisions
	*/
	//CollData* AddColl(const std::string& name);
	/*
	Will return nullptr if collider doesn't exist
	*/
	CollData* GetCollAsset(const std::string& name);
}