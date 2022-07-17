#pragma once

#include "reactphysics3d/reactphysics3d.h"
#include "Engone/AssetModule.h"
#include "Engone/Components/Component.h"

namespace engone {

	
	class GameObject {
	public:
		//GameObject() = default;
		GameObject(engone::Engone* engone) {};
		//~GameObject();

		virtual void update(UpdateInfo& info) {};

		glm::mat4 transform{};
		rp3d::RigidBody* rigidBody=nullptr;
		rp3d::CollisionBody* collisionBody=nullptr;
		ModelAsset* modelAsset=nullptr;
		MeshAsset* meshAsset=nullptr;
		Animator animator;

		//glm::vec3 getLookVector() {
		//	log::out << log::YELLOW << "Warning: GameObject::getLookVector might not work!\n";
		//	float ry = GetCamera()->rotation.y;
		//	float rx = GetCamera()->rotation.x;
		//	return glm::normalize(glm::vec3(sin(ry), sin(rx), cos(ry)));
		//}
		//glm::mat4 getLookMatrix() {
		//	return glm::mat4(1) * glm::rotate(GetCamera()->rotation.y, glm::vec3(0, 1, 0)) * glm::rotate(GetCamera()->rotation.x, glm::vec3(1, 0, 0));
		//}

		glm::mat4 getTransform() const {
			glm::mat4 out(1);
			if (rigidBody) {
				rigidBody->getTransform().getOpenGLMatrix((reactphysics3d::decimal*)&out);
			} else if(collisionBody) {
				collisionBody->getTransform().getOpenGLMatrix((reactphysics3d::decimal*)&out);
			}
			return out;
		}
		virtual void setTransform(glm::mat4 mat) {
			if (collisionBody) {
				rp3d::Transform tr;
				tr.setFromOpenGL((float*)&mat);
				collisionBody->setTransform(tr);
			}
			if (rigidBody) {
				rp3d::Transform tr;
				tr.setFromOpenGL((float*)&mat);
				rigidBody->setTransform(tr);
			}
		}
		virtual void setTransform(glm::vec3 vec) {
			setTransform(glm::translate(glm::mat4(1), vec));
		}
	private:

		friend class Engone;
	};
}