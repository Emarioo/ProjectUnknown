#pragma once

#include "Engone/Utilities/rp3d.h"
#include "Engone/AssetModule.h"
#include "Engone/Components/Component.h"

namespace engone {
	
	class Engone;
	class GameObject {
	public:
		//GameObject() = default;
		GameObject(engone::Engone* engone) {};
		virtual ~GameObject() {}

		virtual void update(UpdateInfo& info) {};

#ifndef ENGONE_NO_PHYSICS
		rp3d::RigidBody* rigidBody=nullptr;
		rp3d::CollisionBody* collisionBody=nullptr;
#endif
		ModelAsset* modelAsset=nullptr;
		MeshAsset* meshAsset=nullptr;
		Animator animator;

		// a matrix, without scale
		glm::mat4 getTransform() const {
#ifndef ENGONE_NO_PHYSICS
			if (rigidBody) 
				return ToMatrix(rigidBody->getTransform());
			if (collisionBody) 
				return ToMatrix(collisionBody->getTransform());
#endif
			return glm::mat4(1);
		}
		virtual void setTransform(glm::mat4 mat) {
#ifndef ENGONE_NO_PHYSICS
			if (collisionBody) {
				collisionBody->setTransform(ToTransform(mat));
			}
			if (rigidBody) {
				rigidBody->setTransform(ToTransform(mat));
			}
#endif
		}
		glm::vec3 getPosition() {
			if(rigidBody)
				return ToGlmVec3(rigidBody->getTransform().getPosition());
			if (collisionBody)
				return ToGlmVec3(collisionBody->getTransform().getPosition());
		}
		virtual void setTransform(glm::vec3 vec) {
			setTransform(glm::translate(glm::mat4(1), vec));
		}
		void setOnlyTrigger(bool yes);

		uint64_t getUUID() const { return m_uuid; }

		// Requires rigidbody and modelAsset to be non-null
		void loadColliders(Engone* engone);

	private:
		uint64_t m_uuid=0;

		friend class Engone;
	};
}