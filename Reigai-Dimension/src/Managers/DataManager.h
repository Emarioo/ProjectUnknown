#pragma once

#include <unordered_map>
#include "World/Dimension.h"
#include "Components/MeshComponent.h"
#include "Components/ColliderComponent.h"
#include "Managers/FileManager.h"
#include <vector>

#include "Utility/Debugger.h"
#include <functional>

/*
Handling data
*/
enum GameState {
	menu,
	play
};
struct TimedFunc {
	TimedFunc(std::function<void()> f,float t) : func(f),time(t){}
	std::function<void()> func;
	float time=0;
};
namespace dManager {

	void AddTimedFunction(std::function<void()> func,float time);
	void Update(float delta);

	GameState GetGameState();
	void SetGameState(GameState);
	/*
	Does nothing at the moment
	*/
	void Init();
	
	void AddAnim(const std::string& file);
	/*
	returns nullptr if name doesn't exist
	*/
	AnimData* GetAnim(const std::string& name);

	void AddBone(const std::string& file);
	/*
	returns nullptr if name doesn't exist
	*/
	BoneData* GetBone(const std::string& name);

	void AddMesh(const std::string& file);
	MeshData* GetMesh(const std::string& name);
	glm::vec3 GetMeshPos(const std::string& name);
	glm::vec3 GetMeshRot(const std::string& name);

	void AddColl(const std::string& file);
	/*
	Add empty collider. Used for objects with unique collisions
	*/
	//CollData* AddColl(const std::string& name);
	/*
	Will return nullptr if collider doesn't exist
	*/
	CollData* GetColl(const std::string& name);

	void AddComponent(MeshComponent* comp);
	void AddComponent(ColliderComponent* comp);
	std::vector<MeshComponent*> *GetMeshComponents();
	std::vector<ColliderComponent*> *GetColliderComponents();
}