#pragma once

#include "Engone/Utilities/rp3d.h"
#include "Engone/AssetModule.h"
#include "Engone/Components/Component.h"

namespace engone {

	
	class GameObject {
	public:
		//GameObject() = default;
		GameObject(engone::Engone* engone) {};
		virtual ~GameObject() {}

		virtual void update(UpdateInfo& info) {};

		glm::mat4 transform{};
#ifndef ENGONE_NO_PHYSICS
		rp3d::RigidBody* rigidBody=nullptr;
		rp3d::CollisionBody* collisionBody=nullptr;
#endif
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
#ifndef ENGONE_NO_PHYSICS
			if (rigidBody) {
				rigidBody->getTransform().getOpenGLMatrix((reactphysics3d::decimal*)&out);
			} else if(collisionBody) {
				collisionBody->getTransform().getOpenGLMatrix((reactphysics3d::decimal*)&out);
			}
#endif
			return out;
		}
		virtual void setTransform(glm::mat4 mat) {
#ifndef ENGONE_NO_PHYSICS
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
#endif
		}
		virtual void setTransform(glm::vec3 vec) {
			setTransform(glm::translate(glm::mat4(1), vec));
		}
		uint64_t getUUID() const { return m_uuid; }

	private:
		uint64_t m_uuid=0;

		friend class Engone;
	};
}