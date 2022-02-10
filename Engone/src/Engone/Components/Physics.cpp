#include "gonpch.h"

#include "Physics.h"

namespace engone {

	bool Physics::IsTriggered() {
		return m_isTriggered;
	}
	bool Physics::TestCollision(Collider& c1, Collider& c2) {
		bool colliding = false;
		if (c1.asset->colliderType == ColliderAsset::Type::Sphere &&
			c2.asset->colliderType == ColliderAsset::Type::Sphere) {

			float dist = (c1.asset->sphere.position - c2.asset->sphere.position).length();
			if (dist < c1.asset->sphere.radius + c2.asset->sphere.radius) {
				colliding = true;
			}

		} else if (c1.asset->colliderType == ColliderAsset::Type::Cube &&
			c2.asset->colliderType == ColliderAsset::Type::Cube) {
			

		}
		if (colliding) {
			if (m_isTrigger) {
				m_isTriggered = true;
				return false;
			} else if(m_isMovable) {
				
				return true;
			}
		}
		return false;
	}
	glm::vec3 IntersectPlane(glm::vec3 plane_p, glm::vec3 plane_n, glm::vec3 lineStart, glm::vec3 lineEnd) {
		//float plane_d = glm::dot(plane_n, plane_p);
		float ad = glm::dot(lineStart, plane_n);
		//float bd = glm::dot(lineEnd, plane_n);
		//float t = (plane_d - ad) / (bd - ad);
		//glm::vec3 lineStartToEnd = lineEnd - lineStart;
		//glm::vec3 lineToIntersect = lineStartToEnd * t;
		//glm::vec3 finish = lineStart + lineToIntersect;
		//return finish;
		return lineStart + (lineEnd - lineStart) * (glm::dot(plane_n, plane_p) - ad) / (glm::dot(lineEnd, plane_n) - ad);
	}
}