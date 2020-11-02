#pragma once

#include <glm/glm.hpp>
#include <Windows.h>

#include "GameObject.h"
#include "Rendering/Camera.h"

class ClientPlayer : public GameObject {
public:
	/*
	SetCamera, SetPosition
	*/
	ClientPlayer();

	Camera* camera;
	void SetCamera(Camera*);

	void Update(float delta);
	bool doMove;
	glm::vec3 Movement(float delta);

	void CreateHitbox();

	bool freeCam = false;
	bool moveCam = true;
	bool sprintMode = false;
	bool crouchMode = false;
	float walkSpeed= 4.0f;
	float sprintSpeed= 7.0f;
	float camSpeed = 1.0f;
	float camFastSpeed = 13.0f;
	float jumpForce = 10.0f;
	
	bool onGround = false;
	float velY = 0;
	float gravity = -0.1f;

private:
};