#pragma once
#include <glm/glm.hpp>

class Camera {
public:
	Camera();
	glm::vec3 position;
	glm::vec3 rotation;

	void SetPosition(float x, float y, float z) {
		position.x = x;
		position.y = y;
		position.z = z;
	}
	void SetPosition(glm::vec3 v) {
		position.x = v.x;
		position.y = v.y;
		position.z = v.z;
	}
	void SetRotation(float x, float y, float z) {
		rotation.x = x;
		rotation.y = y;
		rotation.z = z;
	}
	void SetRotation(glm::vec3 v) {
		rotation.x = v.x;
		rotation.y = v.y;
		rotation.z = v.z;
	}
};