#pragma once

#include <glm/glm.hpp>
#include <Windows.h>

#include "Engine/GameObject.h"
#include "Engine/Objects/ClientPlayer.h"

#include "Inventory/Container.h"

class Player : public engine::ClientPlayer {
public:
	Player(float x,float y,float z);

	void Update(float delta) override;
	glm::vec3 Movement(float delta);

	float health = 20;
	float maxHealth = 100;
	float stamina = 20;
	float maxStamina = 60;
	float hunger = 80;
	float maxHunger = 100;
	float mana = 9;
	float maxMana = 20;

	//Container* inventory;

private:
};