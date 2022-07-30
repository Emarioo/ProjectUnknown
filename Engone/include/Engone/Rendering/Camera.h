#pragma once

// You cannot access position or rotation directly because the view matrix needs to be
// updated whenever the camera changes.
// ISSUE: consider moving definitions to a .cpp file.
class Camera {
public:
	Camera() = default;

	void setPosition(float x, float y, float z) { position.x = x; position.y = y; position.z = z; refreshViewMatrix(); }
	void setPosition(glm::vec3 v) { position = v; refreshViewMatrix(); }
	void setX(float x) { position.x = x; refreshViewMatrix(); }
	void setY(float y) { position.y = y; refreshViewMatrix(); }
	void setZ(float z) { position.z = z; refreshViewMatrix(); }

	void setRotation(float x, float y, float z) { rotation.x = x; rotation.y = y; rotation.z = z; refreshViewMatrix(); }
	void setRotation(glm::vec3 v) { rotation = v; refreshViewMatrix(); }
	void setRX(float x) { rotation.x = x; refreshViewMatrix(); }
	void setRY(float y) { rotation.y = y; refreshViewMatrix(); }
	void setRZ(float z) { rotation.z = z; refreshViewMatrix(); }

	const glm::vec3& getPosition() const { return position; }
	const glm::vec3& getRotation() const { return rotation; }
	const glm::mat4& getViewMatrix() const { return viewMatrix; }

private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::mat4 viewMatrix;

	void refreshViewMatrix() {
		viewMatrix = glm::inverse(
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(rotation.y, glm::vec3(0, 1, 0)) *
			glm::rotate(rotation.x, glm::vec3(1, 0, 0))
		);
	}

};