#pragma once

#include <unordered_map>
#include "World/Dimension.h"
#include "Components/MeshComponent.h"
#include "Components/ColliderComponent.h"
#include "Managers/FileManager.h"
#include <vector>

#include "Utility/Debugger.h"

/*
Handling data
*/
enum GameState {
	menu,
	play
};
namespace dManager {

	GameState GetGameState();
	void SetGameState(GameState);
	/*
	Does nothing at the moment
	*/
	void Init();

	void AddDimension(const std::string& s, Dimension dim);
	Dimension* GetDimension();
	void SetDimension(const std::string&);
	
	void AddAnim(const std::string& name, const std::string& path);
	/*
	returns nullptr if name doesn't exist
	*/
	AnimData* GetAnim(const std::string& name);

	void AddBone(const std::string& name, const std::string& path);
	/*
	returns nullptr if name doesn't exist
	*/
	BoneData* GetBone(const std::string& name);

	void AddMesh(const std::string& name, const std::string& path);
	MeshData* GetMesh(const std::string& name);
	glm::vec3 GetMeshPos(const std::string& name);
	glm::vec3 GetMeshRot(const std::string& name);

	void AddColl(const std::string& name, const std::string& path);
	/*
	Add empty collider. Used for objects with unique collisions
	*/
	CollData* AddColl(const std::string& name);
	/*
	Will return nullptr if collider doesn't exist
	*/
	CollData* GetColl(const std::string& name);

	void AddComponent(MeshComponent* comp);
	void AddComponent(ColliderComponent* comp);
	std::vector<MeshComponent*> *GetMeshComponents();
	std::vector<ColliderComponent*> *GetColliderComponents();
}