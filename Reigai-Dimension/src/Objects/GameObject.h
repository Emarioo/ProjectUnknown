#pragma once

#include "Components/AnimationComponent.h"
#include "Components/MetaData.h"

#include "Data/DataManager.h"

class GameObject{
public:
	GameObject();
	glm::vec3 position;// Pointer to meta strip?
	glm::vec3 rotation;
	glm::vec3 scale;
	//ColliderComponent collider;
	//MaterialType
	/*
	Remember to use delta
	*/
	glm::vec3 velocity;
	std::string name;
	void SetName(std::string name);
	std::string& GetName();
	//MeshData* meshData=nullptr;
	int weight = 1;
	std::string standard_hitbox;


	float proximity = 40;

	//void SetHitbox(std::string);
	virtual void Update(float delta);
	virtual void PreComponents();
	virtual std::vector<ColliderComponent*> GetColliders();

	glm::vec3 GetPos();
	glm::vec3 GetRot();
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetRotationA(float x, float y, float z);
	void SetRotation(glm::vec3 v);
	/*
	return is equal to this.velocity if no collision.
	*/
	glm::vec3 WillCollide(GameObject* o1,float delta);

	void MetaUpdate(float delta);
	MetaData metaData;
	bool hasVelocity = false;
	bool renderHitbox = false;
	bool renderMesh = true;

	bool IsClose(GameObject*);
};