#pragma once

#include "Engone/Components/Entity.h"
#include "Engone/Components/System.h"

class Player : public engone::Entity, public engone::System {
private:
	float animBlending = 0;
	float animSpeed = 1.7;

public:
	Player();

	void Init() override;
	void OnUpdate(float delta) override;
	//void OnCollision(engone::Collider& my, engone::Collider& their) override;
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
};