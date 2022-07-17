#pragma once

#include "../Handlers/AssetHandler.h"

namespace engone {
	class PhysicsG;
	class GameObject;
	struct Collider {
		ColliderAsset* asset;
		PhysicsG* physics;
		GameObject* object;
		
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};
	class PhysicsG {
	private:

	public:
		PhysicsG() = default;
		
		glm::vec3 position;
		glm::vec3 lastPosition;
		glm::vec3 rotation;

		glm::vec3 velocity;

		float gravity = -9.81;

		bool m_isActive = true;
		bool m_renderCollision = true;

		bool m_isTrigger = false;
		bool m_isTriggered = false;
		bool IsTriggered();

		bool m_isMovable = false;

		// @c1 is the owner of the physics
		// @return true is a collision
		bool TestCollision(Collider* c1, Collider* c2);

	};

	glm::vec3 IntersectPlane(glm::vec3 plane_p, glm::vec3 plane_n, glm::vec3 lineStart, glm::vec3 lineEnd);
}