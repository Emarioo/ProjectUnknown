#pragma once

#include "Engone/Utilities/rp3d.h"
#include "Engone/AssetModule.h"
#include "Engone/Assets/Animator.h"
#include "Engone/LoopInfo.h"
#include "Engone/Utilities/RandomUtility.h"

//#include "Engone/Registries/MasterRegistry.h"

namespace engone {
	//class EngineWorld;

	//class EngineObject {
	//public:
	//	// generates a new UUID
	//	EngineObject();
	//	// uuid as 0 will generate a new UUID
	//	EngineObject(UUID uuid);


	//	void setOnlyTrigger(bool yes);
	//	bool isOnlyTrigger();

	//	void setModel(ModelAsset* asset);

	//	// Will load colliders whenever requirements are meet. This method is asynchronous.
	//	// These are: modelAsset is valid, rigidbody is valid.
	//	// The ground parameter is used by the engine and not relevant to you.
	//	void loadColliders(EngineWorld* world = nullptr);

	//	UUID getUUID() const;

	//	//void createRigidBody();

	//	void setObjectType(int type);
	//	int getObjectType();
	//	void setObjectInfo(int info);
	//	int getObjectInfo();

	//private:
	//	int m_objectType = 0;
	//	UUID m_uuid;
	//	ModelAsset* m_modelAsset = nullptr;
	//	rp3d::RigidBody* m_rigidBody = nullptr;
	//	int m_objectInfo=0;

	//	int m_flags=0;

	//	static const int PENDING_COLLIDERS=1;
	//	static const int ONLY_TRIGGER=2;
	//};
	//class ObjectRegistry : public Registry {
	//public:
	//	ObjectRegistry() : Registry("object_registry") {}
	//	
	//	EngineObject* getObject(int objectId);

	//	// returns objectId
	//	int createObject(UUID uuid=0);
	//	void deleteObject();

	//private:
	//	
	//	// int refers to objectType
	//	std::vector<EngineObject> m_objects;
	//};

	//class Engone;
	//class EngineWorld;

	// move function bodies to cpp file. having them in the header feels bad
//	class EngineObject {
//	public:
//		//-- flags
//		//static const uint32_t PENDING_COLLIDERS = 1;
//		//static const uint32_t ONLY_TRIGGER = 2; // all colliders are triggers
//		// make only trigger flag
//
//		//EngineObject() = default;
//		EngineObject() : m_uuid(UUID::New()) {};
//		// uuid as 0 will generate a new UUID
//		EngineObject(UUID uuid) : m_uuid(uuid != 0 ? uuid : UUID::New()) { };
//
//		//virtual void update(LoopInfo& info) {};
//
//		UUID getUUID() const { return m_uuid; }
//		
//		int m_objectType = 0;
//		ModelAsset* m_modelAsset = nullptr;
//		rp3d::RigidBody* m_rigidBody = nullptr;
//		int m_objectInfo;
//		UUID m_uuid;
//
//#ifdef ENGONE_PHYSICS
//		// a matrix, without scale
//		glm::mat4 getTransform() const {
//			if (rigidBody)
//				return ToMatrix(rigidBody->getTransform());
//			return glm::mat4(1);
//		}
//		virtual void setTransform(glm::mat4 mat) {
//			if (rigidBody) {
//				rigidBody->setTransform(ToTransform(mat));
//			}
//		}
//		glm::vec3 getPosition() {
//			if (rigidBody)
//				return ToGlmVec3(rigidBody->getTransform().getPosition());
//			return { 0,0,0 };
//		}
//		virtual void setTransform(glm::vec3 vec) {
//			setTransform(glm::translate(glm::mat4(1), vec));
//		}
//		void setOnlyTrigger(bool yes);
//		bool isOnlyTrigger();
//
//		// loadColliders will be called and will require a rigidBody
//		void setModel(ModelAsset* asset);
//		void createRigidBody(EngineWorld* world);
//
//		// Will load colliders whenever requirements are meet. This method is asynchronous.
//		// These are: modelAsset is valid, rigidbody is valid.
//		// The ground parameter is used by the engine and not relevant to you.
//		void loadColliders(EngineWorld* world = nullptr);
//
//		// sets user data for all colliders of the rigidbody.
//		// if colliders aren't loaded. it will be set when they are.
//		void* colliderData = nullptr;
//		void setColliderUserData(void* data) {
//			colliderData = data;
//			if ((flags & PENDING_COLLIDERS) == 0) { // colliders have been loaded so just set user data here
//				int cols = rigidBody->getNbColliders();
//				for (int i = 0; i < cols; i++) {
//					rigidBody->getCollider(i)->setUserData(colliderData);
//				}
//			}
//		}
//#endif
//
//		int userData = 0;
//
//	private:
//		UUID m_uuid = 0;
//
//		friend class Engone;
//	};
}