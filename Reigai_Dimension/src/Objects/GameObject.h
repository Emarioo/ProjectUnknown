#pragma once

#include "Components/Animation.h"
#include "Components/MetaData.h"
#include "Components/Collision.h"

void pr(std::string s);
void pv(glm::vec3 v);
void pv(glm::vec2 v);
void pf(float v);
void pe();

class GameObject{
public:
	/*
	Remember to use delta
	*/
	glm::vec3 position;// Pointer to meta strip?
	glm::vec3 rotation;
	glm::vec3 scale;
	Collision collision;
	/*
	Remember to use delta
	*/
	glm::vec3 velocity;
	std::string name;
	std::string standard_mesh;
	std::string standard_hitbox;
	float weight;
	GameObject();
	GameObject(float x,float y,float z,std::string mesh);
	//void SetHitbox(std::string);
	virtual void Update(float delta);
	virtual void Draw();

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
	

	void SetName(std::string name);
	std::string& GetName();

	void MetaUpdate(float delta);
	MetaData metaData;
	bool hasVelocity = false;
	bool renderHitbox = true;
	bool renderMesh = true;
};