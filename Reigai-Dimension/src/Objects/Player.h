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

	//Container* inventory;

private:
};