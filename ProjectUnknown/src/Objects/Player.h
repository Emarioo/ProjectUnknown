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
	bool thirdPerson = false;
	bool flight = true;

	bool sprintMode = false;
	bool crouchMode = false;

	float walkSpeed = 4.0f;
	float sprintSpeed = 7.0f;
	float flySpeed = 2.0f*5;
	float flyFastSpeed = 13.0f*10;
	float jumpForce = 10.0f;

	bool onGround = false;

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