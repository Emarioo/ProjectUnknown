#pragma once
#include <glm/glm.hpp>

class Camera {
public:
	Camera() {}
	glm::vec3 position=glm::vec3(0,0,0);
	glm::vec3 rotation=glm::vec3(0,0,0);

	void SetPos(float x, float y, float z) {
		position.x = x;
		position.y = y;
		position.z = z;
	}
	void SetRot(float x, float y, float z) {
		rotation.x = x;
		rotation.y = y;
		rotation.z = z;
	}
	//glm::vec3 LookVector();
};