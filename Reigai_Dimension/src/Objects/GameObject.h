#pragma once

#include "Components/Animation.h"
#include "Components/MetaData.h"

class GameObject;
class Location {
public:
	Location();
	glm::mat4 loc;
	void Translate(glm::vec3 v);
	void Rotate(glm::vec3 v);
	void Scale(glm::vec3 v);
	glm::mat4 mat();
	glm::vec3 vec();
};
class Collision {// needs improvment
public:
	Collision();
	//void SetHitbox(float,float,float,float,float,float);
	//bool isSolid = false;
	//glm::vec3 *pos;
	//float x, y, z, w, h, d;

	// Functions to create cubes, lines... or read from collision file from blender

	void MakeCube(float x,float y,float z,float w,float h,float d);
	void MakeFace(glm::vec3 p1,glm::vec3 n1,glm::vec3 n2,glm::vec3 n3);
	void MakeSphere(float radius);
	//void AddTri(glm::vec3 p1,glm::vec3 p2,glm::vec3 p3);
	int type = 0;// 0 = planes, 1 = sphere
	glm::vec3* oPos = nullptr;
	glm::vec3* oRot = nullptr;
	//glm::vec3 origin=glm::vec3(0,0,0);
	std::vector<glm::vec3> originPoints;
	std::vector<glm::vec3> points;
	void UpdatePoints(glm::vec3 r);
	std::vector<int> quadIndex;// 4 ints is one quad
	float radius=0;

	bool print = false;

	bool IsClose(Collision*);// Is object close?
	/*
	Argument is Collision's Lines to test against This quads
	*/
	glm::vec3 collision(Collision*);// how much to move back

	BufferContainer outline;
	void Draw();

	//float center(int a);
	//float sideX(bool b);
	//float sideY(bool b);
	//float sideZ(bool b);
	/*
	This is set in Management when AddUpdateO is called. (AddUpdateO does not run for ClientPlayer so objects has to be set manually)
	*/
	//std::vector<GameObject*>* objects;

	//std::vector<GameObject*> IsCollidingL();
	//GameObject* IsColliding();
};
class GameObject{
private:
public:
	/*
	Remember to use delta
	*/
	glm::vec3 position;// Pointer to meta strip?
	glm::vec3 rotation;
	glm::vec3 scale;
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

	glm::vec3 GetPos() {
		return position;
	}
	glm::vec3 GetRot() {
		return rotation;
	}
	void SetPosition(float x, float y, float z) {
		position.x = x;
		position.y = y;
		position.z = z;
	}
	void SetRotation(float x, float y, float z) {
		rotation.x = x;
		rotation.y = y;
		rotation.z = z;
		collision.UpdatePoints(rotation);
	}
	void SetRotation(glm::vec3 v) {
		rotation.x = v.x;
		rotation.y = v.y;
		rotation.z = v.z;
		collision.UpdatePoints(rotation);
	}
	/*
	return is equal to this.velocity if no collision.
	*/
	glm::vec3 WillCollide(GameObject* o1,float delta);
	Collision collision;
	bool isSolid = false;

	void SetName(std::string name);
	std::string& GetName();

	void MetaUpdate(float delta);
	MetaData metaData;
	bool hasVelocity = false;
	bool renderHitbox = true;
	bool renderMesh = true;
};