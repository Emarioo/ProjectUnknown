#pragma once

class Camera {
public:
	Camera();
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 rotation;

	void SetPosition(float x, float y, float z);
	void SetPosition(glm::vec3 v);
	void SetRotation(float x, float y, float z);
	void SetRotation(glm::vec3 v);
};