#include "gonpch.h"

#include "Physics.h"

namespace engone {

	bool Physics::IsTriggered() {
		return m_isTriggered;
	}
	bool Physics::TestCollision(Collider& c1, Collider& c2) {
		bool colliding = false;
		float time = 0;
		if (c1.asset->colliderType == ColliderAsset::Type::Sphere &&
			c2.asset->colliderType == ColliderAsset::Type::Sphere) {

			float dist = glm::length(c1.asset->sphere.position - c2.asset->sphere.position);
			if (dist < c1.asset->sphere.radius + c2.asset->sphere.radius) {
				colliding = true;
				time = 1;
			}

		} else if (c1.asset->colliderType == ColliderAsset::Type::Cube &&
			c2.asset->colliderType == ColliderAsset::Type::Cube) {
			
			if (c1.asset->cube.rotation == glm::vec3(0)&& c2.asset->cube.rotation == glm::vec3(0)){
				if (c1.physics->position.x+c1.asset->cube.scale.x < c2.physics->position.x&&
					c1.physics->position.x > c2.physics->position.x+c2.asset->cube.scale.x&&
					c1.physics->position.y + c1.asset->cube.scale.y < c2.physics->position.y &&
					c1.physics->position.y > c2.physics->position.y + c2.asset->cube.scale.y&&
					c1.physics->position.z + c1.asset->cube.scale.z < c2.physics->position.z &&
					c1.physics->position.z > c2.physics->position.z + c2.asset->cube.scale.z) {

					colliding = true;
					time = 1;
				}
			}
		}
		if (colliding) {
			if (m_isTrigger) {
				m_isTriggered = true;
				return false;
			} else if(m_isMovable) {
				c1.physics->position -= c1.physics->velocity * time;
				c1.physics->velocity = {0,0,0};
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