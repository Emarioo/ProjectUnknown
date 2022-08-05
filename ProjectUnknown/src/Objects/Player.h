#pragma once

#include "Engone/Engone.h"
#include "Engone/GameObject.h"

class Player : public engone::GameObject {
public:
	Player(engone::Engone* engone);

	void update(engone::UpdateInfo& info) override;
	void Movement(engone::UpdateInfo& info);
	void Input(engone::UpdateInfo& info);

	// Movement and camera
	float zoomOut = 0;

	//glm::vec3 collisionNormal = {0,0,0};
	//glm::vec3 lastForce = {0,0,0};

	//bool sprintMode = false;
	//bool crouchMode = false;

	engone::GameObject* inventorySword = nullptr;
	engone::GameObject* heldWeapon = nullptr;
	rp3d::Joint* weaponJoint = nullptr;
	void setWeapon(engone::GameObject* weapon);

	bool noclip = false;

	float walkSpeed = 6.f;
	float sprintSpeed = 12.f;
	float jumpForce = 0.5f;

	engone::Camera testCam;

	//float lastVelocity = 0;
	bool onGround = false;

	// a little odd to have it here the class needs it when making joints
	engone::Engone* engone=nullptr;

private:
	float animBlending = 0;
	float animSpeed = 1.7f;

	
};