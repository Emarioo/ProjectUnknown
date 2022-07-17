#pragma once

#include "Engone/Components/Collider.h"

namespace engone {
	float insideLine(glm::vec3& a, glm::vec3& b, glm::vec3& p);
	float leng2(glm::vec3 v);
	float closestLength(glm::vec3 a, glm::vec3 b, glm::vec3 p);
	glm::vec3 IntersectPlane(glm::vec3 plane_p, glm::vec3 plane_n, glm::vec3 lineStart, glm::vec3 lineEnd);
	bool TestComplexCollision(Collider* c1, Collider* c2);
	bool TestCollision(Collider* c1, Collider* c2);

	bool TestPlanes(PlaneCollider& a, PlaneCollider& b);
}