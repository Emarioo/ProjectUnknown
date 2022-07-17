
#include "Engone/Rendering/Camera.h"

Camera::Camera() : 
	position({ 0,0,0 }), velocity({0,0,0}), rotation({ 0,0,0 })
{}
void Camera::setPosition(float x, float y, float z) {
	position.x = x;
	position.y = y;
	position.z = z;
}
void Camera::setPosition(glm::vec3 v) {
	position.x = v.x;
	position.y = v.y;
	position.z = v.z;
}
void Camera::setRotation(float x, float y, float z) {
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
}
void Camera::setRotation(glm::vec3 v) {
	rotation.x = v.x;
	rotation.y = v.y;
	rotation.z = v.z;
}