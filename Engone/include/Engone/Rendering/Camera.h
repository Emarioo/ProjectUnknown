#pragma once

class Camera {
public:
	Camera();
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 rotation;

	void setPosition(float x, float y, float z);
	void setPosition(glm::vec3 v);
	void setRotation(float x, float y, float z);
	void setRotation(glm::vec3 v);
};