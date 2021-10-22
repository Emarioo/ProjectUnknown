#pragma once

#include "Engone/Objects/GameObject.h"

class Player : public engone::GameObject {
private:
	float animBlending = 0;
	float animSpeed = 1.7;

public:
	Player(float x,float y,float z);

	void Update(float delta) override;
	glm::vec3 Movement(float delta);

	// Movement and camera
	//bool freeCam = false; // use GameState CameraToPlayer
	bool thirdPerson = false;
	bool flight = false;

	bool sprintMode = false;
	bool crouchMode = false;

	bool freeCamT = false;
	bool thirdPersonT = false;
	bool flightT = false;

	bool crouchT = false;

	float walkSpeed = 4.0f;
	float sprintSpeed = 7.0f;
	float camSpeed = 2.0f*5;
	float camFastSpeed = 13.0f*10;
	float jumpForce = 10.0f;

	bool onGround = false;
	float gravity = -0.3f;

	// Stats
	float health = 20;
	float maxHealth = 100;
	float stamina = 20;
	float maxStamina = 60;
	float hunger = 80;
	float maxHunger = 100;
	float mana = 9;
	float maxMana = 20;
};