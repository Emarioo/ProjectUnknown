#include "gonpch.h"

#include "Camera.h"

Camera::Camera() : 
	position({ 0,0,0 }), velocity({0,0,0}), rotation({ 0,0,0 })
{}
void Camera::SetPosition(float x, float y, float z) {
	position.x = x;
	position.y = y;
	position.z = z;
}
void Camera::SetPosition(glm::vec3 v) {
	position.x = v.x;
	position.y = v.y;
	position.z = v.z;
}
void Camera::SetRotation(float x, float y, float z) {
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
}
void Camera::SetRotation(glm::vec3 v) {
	rotation.x = v.x;
	rotation.y = v.y;
	rotation.z = v.z;
}