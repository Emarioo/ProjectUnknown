#pragma once

#include "Engone/Engone.h"
#include "Engone/GameObject.h"

class Player : public engone::GameObject {
public:
	Player(engone::Engone* engone);

	void update(engone::UpdateInfo& info) override;
	void Movement(float delta);

	// Movement and camera
	float zoomOut = 0;

	bool sprintMode = false;
	bool crouchMode = false;

	float walkSpeed = 4.0f;
	float sprintSpeed = 7.0f;
	float flySpeed = 2.0f*5;
	float flyFastSpeed = 13.0f*10;
	float jumpForce = 10.0f;

	float lastVelocity = 0;
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

private:
	float animBlending = 0;
	float animSpeed = 1.7f;

	
};