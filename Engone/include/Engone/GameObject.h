#pragma once

#include "Engone/Utilities/rp3d.h"
#include "Engone/AssetModule.h"
#include "Engone/Assets/Animator.h"
#include "Engone/LoopInfo.h"
#include "Engone/Utilities/RandomUtility.h"

namespace engone {
	
	class Engone;
	class GameGround;
	class GameObject {
	public:
		static const int PENDING_COLLIDERS=1;

		//GameObject() = default;
		GameObject() : m_uuid(UUID::New()) {};
		GameObject(UUID uuid) : m_uuid(uuid) {};
		virtual ~GameObject() {}

		virtual void update(UpdateInfo& info) {};

		UUID getUUID() const { return m_uuid; }
		ModelAsset* modelAsset=nullptr;
		Animator animator;
		int flags; // make it private and provide methods?

#ifdef ENGONE_PHYSICS
		rp3d::RigidBody* rigidBody=nullptr;
		rp3d::CollisionBody* collisionBody=nullptr;
		// a matrix, without scale
		glm::mat4 getTransform() const {
			if (rigidBody) 
				return ToMatrix(rigidBody->getTransform());
			if (collisionBody) 
				return ToMatrix(collisionBody->getTransform());
			return glm::mat4(1);
		}
		virtual void setTransform(glm::mat4 mat) {
			if (collisionBody) {
				collisionBody->setTransform(ToTransform(mat));
			}
			if (rigidBody) {
				rigidBody->setTransform(ToTransform(mat));
			}
		}
		glm::vec3 getPosition() {
			if(rigidBody)
				return ToGlmVec3(rigidBody->getTransform().getPosition());
			if (collisionBody)
				return ToGlmVec3(collisionBody->getTransform().getPosition());
			return { 0,0,0 };
		}
		virtual void setTransform(glm::vec3 vec) {
			setTransform(glm::translate(glm::mat4(1), vec));
		}
		void setOnlyTrigger(bool yes);

		// Will load colliders whenever requirements are meet. This method is asynchronous.
		// These are: modelAsset is valid, rigidbody is valid.
		// The ground parameter is used by the engine and not relevant to you.
		void loadColliders(GameGround* ground=nullptr);
#endif

	private:
		UUID m_uuid=0;

		friend class Engone;
	};
}