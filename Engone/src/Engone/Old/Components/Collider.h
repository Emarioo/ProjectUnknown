#pragma once

#include "Engone/AssetModule.h"
#include "Engone/Components/Entity.h"

namespace engone {
	struct Collider {
		ColliderAsset* asset;
		Entity entity;

		glm::mat4 matrix;
		glm::vec3 position;
		glm::vec3 scale;
	};
	struct PlaneCollider {
		PlaneCollider() = default;
		PlaneCollider(int pointCount, int planeCount) : points(pointCount), planes(planeCount) {  };
		std::vector<glm::vec3> points;
		struct plane {
			int a, b, c; // indices to points
		};
		std::vector<plane> planes;

		glm::vec3 getPoint(int plane, int point) {
			if(point==0) return points[planes[plane].a];
			else if(point==1) return points[planes[plane].b];
			return points[planes[plane].c];
		}
	};

	inline PlaneCollider MakeCubeCollider(glm::mat4 mat, glm::vec3 scale) {
		PlaneCollider collider(8,12);
		collider.points[0] = (mat * glm::translate(glm::vec3(0, 0, 0)))[3];
		collider.points[1] = (mat * glm::translate(glm::vec3(scale.x, 0, 0)))[3];
		collider.points[2] = (mat * glm::translate(glm::vec3(0, scale.y, 0)))[3];
		collider.points[3] = (mat * glm::translate(glm::vec3(scale.x, scale.y, 0)))[3];
		collider.points[4] = (mat * glm::translate(glm::vec3(0, 0, scale.z)))[3];
		collider.points[5] = (mat * glm::translate(glm::vec3(scale.x, 0, scale.z)))[3];
		collider.points[6] = (mat * glm::translate(glm::vec3(0, scale.y, scale.z)))[3];
		collider.points[7] = (mat * glm::translate(glm::vec3(scale.x, scale.y, scale.z)))[3];
		// bottom
		collider.planes[0] = {0,1,2};
		collider.planes[1] = {1,2,3};
		// front
		collider.planes[2] = {0,1,4};
		collider.planes[3] = {1,4,5};
		// left
		collider.planes[4] = {0,2,4};
		collider.planes[5] = {2,4,6};
		// right
		collider.planes[6] = {1,3,5};
		collider.planes[7] = {3,5,7};
		// back
		collider.planes[8] = {2,3,6};
		collider.planes[9] = {3,6,7};
		// top
		collider.planes[10] = {4,5,6};
		collider.planes[11] = {5,6,7};
		return collider;
	}
	
}