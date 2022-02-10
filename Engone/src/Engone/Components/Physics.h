#pragma once

#include "../Handlers/AssetHandler.h"

namespace engone {
	class Physics;
	struct Collider {
		glm::mat4 matrix;
		ColliderAsset* asset;
		Physics* physics;
	};
	class Physics {
	private:

	public:
		Physics() = default;
		
		glm::vec3 position;
		glm::vec3 rotation;

		glm::vec3 velocity;

		float gravity = -9.81;

		bool m_isActive = true;
		bool m_renderCollision = true;

		bool m_isTrigger = false;
		bool m_isTriggered = false;
		bool IsTriggered();

		bool m_isMovable = true;

		// @c1 is the owner of the physics
		// @return as false should allow further calculations on other colliders
		bool TestCollision(Collider& c1, Collider& c2);


	};

	glm::vec3 IntersectPlane(glm::vec3 plane_p, glm::vec3 plane_n, glm::vec3 lineStart, glm::vec3 lineEnd);
}