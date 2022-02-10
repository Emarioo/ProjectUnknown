#pragma once

#include "../Handlers/AssetHandler.h"

namespace engone {
	struct Collider {
		glm::mat4 matrix;
		ColliderAsset* asset;
	};
	class CollisionComponent {
	private:
		bool m_isTriggered=false;

	public:
		CollisionComponent() = default;
		
		bool m_renderMesh=false;
		
		bool m_isActive = true;

		bool m_isTrigger = false;
		bool m_isMovable = true;

		// @c1 is a collider from the owner of collisionComponent
		// @return as true will continue calculations on other colliders
		bool TestCollision(Collider& c1, Collider& c2);
		
		bool IsTriggered();

	};
}