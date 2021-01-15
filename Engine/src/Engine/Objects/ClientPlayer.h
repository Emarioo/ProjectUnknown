#pragma once

#include <glm/glm.hpp>
#include <Windows.h>

#include "GameObject.h"
#include "Rendering/Camera.h"
namespace engine {
	class ClientPlayer : public GameObject {
	public:
		/*
		SetCamera, SetPosition
		*/
		/*
		Setup player Components
		*/
		ClientPlayer();
		Camera* camera;
		void SetCamera(Camera* cam);

		void PreComponents() override;
		void Update(float delta) override;
		std::vector<ColliderComponent*> GetColliders() override;
		bool doMove;
		glm::vec3 Movement(float delta);

		engine::RenderComponent mesh;
		BoneComponent bone;
		AnimationComponent animation;
		ColliderComponent collider;

		bool freeCam = false;
		bool moveCam = true;
		bool sprintMode = false;
		bool crouchMode = false;
		float walkSpeed = 4.0f;
		float sprintSpeed = 7.0f;
		float camSpeed = 2.0f;
		float camFastSpeed = 13.0f;
		float jumpForce = 10.0f;

		bool onGround = false;
		float velY = 0;
		float gravity = -0.1f;

	private:
	};
}