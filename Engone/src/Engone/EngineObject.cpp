#include "Engone/EngineObject.h"
#include "Engone/Engone.h"

namespace engone {
	EngineObject::EngineObject() : m_uuid(UUID::New()) {};
	EngineObject::EngineObject(UUID uuid) : m_uuid(uuid != 0 ? uuid : UUID::New()) { };

	void EngineObject::setOnlyTrigger(bool yes) {
		if (m_rigidBody) {
			if (yes)
				m_flags |= ONLY_TRIGGER;
			else
				m_flags &= ~ONLY_TRIGGER;
			for (int i = 0; i < m_rigidBody->getNbColliders(); i++) {
				auto col = m_rigidBody->getCollider(i);
				col->setIsTrigger(yes);
			}
		}
	}
	bool EngineObject::isOnlyTrigger() {
		return m_flags & ONLY_TRIGGER;
	}
	void EngineObject::setModel(ModelAsset* asset) {
		if (m_modelAsset == asset)
			return;

		// cleanup previous model
		if (m_modelAsset) {
			if (m_rigidBody) {
				while (m_rigidBody->getNbColliders() != 0) {
					rp3d::Collider* col = m_rigidBody->getCollider(0);
					m_rigidBody->removeCollider(col);
				}
			}
			//animator.cleanup();
			m_modelAsset = nullptr;
		}

		// set new model
		m_modelAsset = asset;
		//animator.asset = modelAsset;
		loadColliders();
	}
	//void EngineObject::createRigidBody() {
	//	if (m_rigidBody)
	//		return; // already have rigidBody

	//	//rp3d::Transform t;
	//	//if (world->m_pWorld) {
	//	//	rigidBody = world->m_pWorld->createRigidBody(t);
	//	//} else {
	//	//	log::out << log::RED << "EngineObject : createRigidBody - physics world was nullptr\n";
	//	//}
	//}
	rp3d::RigidBody* EngineObject::getRigidBody() {
		return m_rigidBody;
	}
	void EngineObject::setObjectType(int type) {
		m_objectType = type;
	}
	int EngineObject::getObjectType() {
		return m_objectType;
	}
	void EngineObject::setObjectInfo(int info) {
		m_objectInfo = info;
	}
	int EngineObject::getObjectInfo() {
		return m_objectInfo;
	}
	void EngineObject::loadColliders(EngineWorld* world = nullptr) {
		if (!m_modelAsset) {
			log::out << log::RED << "EngineObject : Cannot load colliders when modelAsset is null\n";
			return;
		}
		if (!m_rigidBody) {
			log::out << log::RED << "EngineObject : Cannot load colliders when rigidBody is null\n";
			return;
		}
		if (!m_modelAsset->valid() || world == nullptr) {
			m_flags |= PENDING_COLLIDERS; // try again in update loop of engine.
			return;
		}

		std::vector<glm::mat4> transforms = m_modelAsset->getParentTransforms(nullptr);

		bool noColliders = true;
		for (int i = 0; i < m_modelAsset->instances.size(); i++) {
			AssetInstance& inst = m_modelAsset->instances[i];
			//log::out << inst.name << "\n";
			//log::out <<loc << "\n";
			if (inst.asset->type == ColliderAsset::TYPE) {
				noColliders = false;
				ColliderAsset* asset = (ColliderAsset*)inst.asset;
				glm::mat4 loc = transforms[i] * inst.localMat;
				glm::vec3 matScale;
				rp3d::Transform tr = ToTransform(loc, &matScale);
				rp3d::CollisionShape* shape = nullptr;

				if (asset->colliderType == ColliderAsset::MapType) {
					glm::vec3 scale = asset->map.scale;
					scale.x *= matScale.x;
					scale.y *= matScale.y;
					scale.z *= matScale.z;
					shape = world->getPhysicsCommon()->createHeightFieldShape(asset->map.gridColumns,
						asset->map.gridRows, asset->map.minHeight, asset->map.maxHeight,
						asset->map.heights.data(), rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE,
						1, 1, *(rp3d::Vector3*)&scale);

				} else if (asset->colliderType == ColliderAsset::CubeType) {
					glm::mat4 loc = transforms[i] * inst.localMat;
					rp3d::Vector3 scale = *(rp3d::Vector3*)&asset->cube.size;
					scale.x *= matScale.x;
					scale.y *= matScale.y;
					scale.z *= matScale.z;
					shape = world->getPhysicsCommon()->createBoxShape(scale);
				} else if (asset->colliderType == ColliderAsset::CapsuleType) {
					float realHeight = asset->capsule.height - asset->capsule.radius * 2;
					realHeight *= matScale.y;
					float radius = asset->capsule.radius;
					radius *= (matScale.x + matScale.z) / 2;
					shape = world->getPhysicsCommon()->createCapsuleShape(radius, realHeight);
				} else if (asset->colliderType == ColliderAsset::SphereType) {
					float radius = asset->sphere.radius;
					radius *= (matScale.x + matScale.y + matScale.z) / 3.f;
					shape = world->getPhysicsCommon()->createSphereShape(radius);
				}

				m_rigidBody->addCollider(shape, tr);
				auto col = m_rigidBody->getCollider(m_rigidBody->getNbColliders() - 1);
				col->getMaterial().setFrictionCoefficient(0.5f);
				col->getMaterial().setBounciness(0.0f);
				col->setUserData(m_colliderData);
				col->setIsTrigger(m_flags & ONLY_TRIGGER);
			}
		}
		m_flags &= ~PENDING_COLLIDERS;
		if (noColliders) {
			// updating center of mass would cause wierd stuff
			log::out << "EngineObject : Loaded no colliders\n";
		} else {
			m_rigidBody->updateLocalCenterOfMassFromColliders();
			m_rigidBody->updateLocalInertiaTensorFromColliders();
		}
	}
	UUID EngineObject::getUUID() const {
		return m_uuid;
	}
	void EngineObject::setColliderUserData(void* data) {
		m_colliderData = data; 
		if ((m_flags & PENDING_COLLIDERS) == 0) { // colliders have been loaded so just set user data here
			int cols = m_rigidBody->getNbColliders();
			for (int i = 0; i < cols; i++) {
				m_rigidBody->getCollider(i)->setUserData(m_colliderData);
			}
		} 
	}
//#ifdef ENGONE_PHYSICS
//	void EngineObject::setModel(ModelAsset* asset) {
//		if (modelAsset == asset)
//			return;
//
//		// cleanup previous model
//		if (modelAsset) {
//			while(rigidBody->getNbColliders()!=0) {
//				rp3d::Collider* col = rigidBody->getCollider(0);
//				rigidBody->removeCollider(col);
//			}
//			animator.cleanup();
//			modelAsset = nullptr;
//		}
//		
//		// set new model
//		modelAsset = asset;
//		animator.asset = modelAsset;
//		loadColliders();
//	}
//	void EngineObject::createRigidBody(EngineWorld* world) {
//		rp3d::Transform t;
//		if (world->m_pWorld) {
//			rigidBody = world->m_pWorld->createRigidBody(t);
//		} else {
//			log::out << log::RED << "EngineObject::createRigidBody - physics world was nullptr\n";
//		}
//	}
//	void EngineObject::loadColliders(EngineWorld* world) {
//		if (!modelAsset) {
//			log::out << log::RED << "EngineObject::loadColliders - ModelAsset is null\n";
//			return;
//		}
//		if (!rigidBody) {
//			log::out << log::RED << "EngineObject::loadColliders - Rigidbody is null\n";
//			return;
//		}
//		if (!modelAsset->valid()|| world ==nullptr) {
//			flags |= PENDING_COLLIDERS; // try again in update loop of engine.
//			return;
//		}
//
//		std::vector<glm::mat4> transforms = modelAsset->getParentTransforms(nullptr);
//
//		bool noColliders = true;
//		for (int i = 0; i < modelAsset->instances.size(); i++) {
//			AssetInstance& inst = modelAsset->instances[i];
//			//log::out << inst.name << "\n";
//			//log::out <<loc << "\n";
//			if (inst.asset->type == ColliderAsset::TYPE) {
//				noColliders = false;
//				ColliderAsset* asset = (ColliderAsset*)inst.asset;
//				glm::mat4 loc = transforms[i]*inst.localMat;
//				glm::vec3 matScale;
//				rp3d::Transform tr = ToTransform(loc,&matScale);
//				rp3d::CollisionShape* shape = nullptr;
//
//				if (asset->colliderType == ColliderAsset::MapType) {
//					glm::vec3 scale = asset->map.scale;
//					scale.x*=matScale.x;
//					scale.y*=matScale.y;
//					scale.z*=matScale.z;
//					shape = world->m_pCommon->createHeightFieldShape(asset->map.gridColumns,
//						asset->map.gridRows, asset->map.minHeight, asset->map.maxHeight,
//						asset->map.heights.data(), rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE,
//						1, 1, *(rp3d::Vector3*)&scale);
//
//				} else if (asset->colliderType == ColliderAsset::CubeType) {
//					glm::mat4 loc = transforms[i] * inst.localMat;
//					rp3d::Vector3 scale = *(rp3d::Vector3*)&asset->cube.size;
//					scale.x*=matScale.x;
//					scale.y*=matScale.y;
//					scale.z*=matScale.z;
//					shape = world->m_pCommon->createBoxShape(scale);
//				}
//				else if (asset->colliderType == ColliderAsset::CapsuleType) {
//					float realHeight = asset->capsule.height - asset->capsule.radius * 2;
//					realHeight *= matScale.y;
//					float radius = asset->capsule.radius;
//					radius *= (matScale.x + matScale.z) / 2;
//					shape = world->m_pCommon->createCapsuleShape(radius, realHeight);
//				}
//				else if (asset->colliderType == ColliderAsset::SphereType) {
//					float radius = asset->sphere.radius;
//					radius *= (matScale.x + matScale.y + matScale.z) / 3.f;
//					shape = world->m_pCommon->createSphereShape(radius);
//				}
//
//				rigidBody->addCollider(shape, tr);
//				auto col = rigidBody->getCollider(rigidBody->getNbColliders() - 1);
//				col->getMaterial().setFrictionCoefficient(0.5f);
//				col->getMaterial().setBounciness(0.0f);
//				col->setUserData(colliderData);
//				col->setIsTrigger(flags & ONLY_TRIGGER);
//			}
//		}
//		flags &= ~PENDING_COLLIDERS;
//		if (noColliders) {
//			// updating center of mass would cause wierd stuff
//			log::out << "Loaded no colliders\n";
//		} else {
//			rigidBody->updateLocalCenterOfMassFromColliders();
//			rigidBody->updateLocalInertiaTensorFromColliders();
//		}
//	}
//	void EngineObject::setOnlyTrigger(bool yes) {
//		if (rigidBody) {
//			if (yes) flags |= ONLY_TRIGGER;
//			else flags &= ~ONLY_TRIGGER;
//			for (int i = 0; i < rigidBody->getNbColliders(); i++) {
//				auto col = rigidBody->getCollider(i);
//				col->setIsTrigger(yes);
//			}
//		}
//	}
//	bool EngineObject::isOnlyTrigger() {
//		return flags&ONLY_TRIGGER;
//	}
//#endif
}