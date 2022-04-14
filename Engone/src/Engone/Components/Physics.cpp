
#include "Physics.h"

namespace engone {

	bool PhysicsG::IsTriggered() {
		return m_isTriggered;
	}
	bool PhysicsG::TestCollision(Collider* c1, Collider* c2) {
		bool colliding = false;
		float time = 0;
		if (c1->asset->colliderType == ColliderAsset::Type::Sphere &&
			c2->asset->colliderType == ColliderAsset::Type::Sphere) {

			glm::vec3& rot1 = c1->asset->sphere.position;
			float dist = glm::length(c1->asset->sphere.position - c2->asset->sphere.position);
			if (dist < c1->asset->sphere.radius + c2->asset->sphere.radius) {
				colliding = true;
				time = 1;
			}

		} else if (c1->asset->colliderType == ColliderAsset::Type::Cube &&
			c2->asset->colliderType == ColliderAsset::Type::Cube) {
			
			// get collision points
			glm::vec3 move = c1->physics->position - c1->physics->lastPosition;
			if (move.x == 0 && move.y == 0 && move.z == 0) {
				move = c2->physics->position - c2->physics->lastPosition;
				if (move.x == 0 && move.y == 0 && move.z == 0)
					return false;
				Collider* temp=c1;
				c1 = c2;
				c2 = temp;
			}

			glm::vec3 diff = c1->physics->lastPosition - c2->physics->lastPosition;
			glm::vec3 point1 = c1->position-c1->scale/2.f;
			glm::vec3 point2 = c2->position-c2->scale/2.f;
			
			point1.x += (diff.x < 0) ? c1->scale.x:0;
			point1.y += (diff.y < 0) ? c1->scale.y:0;
			point1.z += (diff.z < 0) ? c1->scale.z:0;
			point2.x += (diff.x < 0) ? 0:c2->scale.x;
			point2.y += (diff.y < 0) ? 0:c2->scale.y;
			point2.z += (diff.z < 0) ? 0:c2->scale.z;

			float tx = (point2.x - point1.x) / move.x;
			float ty = (point2.y - point1.y) / move.y;
			float tz = (point2.z - point1.z) / move.z;
			
			glm::vec3 s1 = c1->scale;
			glm::vec3 p1 = c1->position - s1 / 2.f;
			glm::vec3 s2 = c2->scale;
			glm::vec3 p2 = c2->position - s2 / 2.f;

			if (p1.x + s1.x > p2.x && p1.x < p2.x + s2.x &&
				p1.y + s1.y > p2.y && p1.y < p2.y + s2.y &&
				p1.z + s1.z > p2.z && p1.z < p2.z + s2.z) {

				if (abs(tx) < abs(ty) && abs(tx) < abs(tz)) {
					c1->physics->position.x += tx * move.x;
					c1->physics->velocity.x = 0;
					return true;
				} else if (abs(ty) < abs(tz)) {
					c1->physics->position.y += ty * move.y;
					c1->physics->velocity.y = 0;
					return true;
				} else {
					c1->physics->position.z += tz * move.z;
					c1->physics->velocity.z = 0;
					return true;
				}
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