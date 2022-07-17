#pragma once

#include <glm/glm.hpp>
#include <Windows.h>

#include "../GameObject.h"
#include "../Rendering/Camera.h"
namespace engine {
	class ClientPlayer : public GameObject {
	public:
		ClientPlayer();

		void Update(float delta) override;
		virtual glm::vec3 Movement(float delta);

		bool freeCam = false;
		bool thirdPerson = false;
		bool flight = false;

		bool sprintMode = false;
		bool crouchMode = false;

		bool freeCamT= false;
		bool thirdPersonT= false;
		bool flightT = false;

		bool crouchT = false;

		float walkSpeed = 4.0f;
		float sprintSpeed = 7.0f;
		float camSpeed = 2.0f;
		float camFastSpeed = 13.0f;
		float jumpForce = 10.0f;

		bool onGround = false;
		float gravity = -0.3f;

	private:
	};
}