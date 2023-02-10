#include "Engone/EngineObject.h"
#include "Engone/Engone.h"
#include "Engone/World/EngineWorld.h"

namespace engone {
	EngineObject::~EngineObject() {
		cleanup();
	}
	void EngineObject::cleanup() {
		if (m_objectInfo != 0) {
			log::out << log::RED << "EngineObject::cleanup - Object Info is "<<m_objectInfo<<", did you forget to free memory?\n";
		}
#ifdef ENGONE_PHYSICS
		if (m_rigidBody) {
			m_world->lockPhysics();
			m_world->getPhysicsWorld()->destroyRigidBody(m_rigidBody);
			//log::out << "destroy rigidbody " << (void*)m_rigidBody << "\n";
			m_world->unlockPhysics();
			m_rigidBody = nullptr;
		}
#endif
		removeAnimator();
	}
	void EngineObject::init(EngineWorld* world, UUID uuid) {
		if (!world) {
			log::out << log::RED << "EngineObject : World is null\n";
			return;
		}
		m_world = world;
		if (uuid == 0)
			m_uuid = UUID::New();
		else
			m_uuid = uuid;
#ifdef ENGONE_PHYSICS
		if (!world->getPhysicsWorld()) {
			log::out << log::RED << "EngineObject : Physics world is null\n";
			return;
		}
		rp3d::Transform t;
		m_world->lockPhysics();
		m_rigidBody = world->getPhysicsWorld()->createRigidBody(t); // PHYSICS WORLD DOES NOT HAVE MUTEX, DANGEROUS
		//log::out << "create rigidbody " << (void*)m_rigidBody << "\n";
		m_world->unlockPhysics();
		if (!m_rigidBody) {
			log::out << log::RED << "EngineObject : RigidBody is null\n";
			return;
		}
		m_rigidBody->setUserData(this);
#endif
	}

#ifdef ENGONE_PHYSICS
	void EngineObject::setOnlyTrigger(bool yes) {
		return;
		if (!m_rigidBody)
			return;
		if ((m_flags & ONLY_TRIGGER) && yes)
			return;
			
		if (yes) m_flags |= ONLY_TRIGGER;
		else m_flags &= ~ONLY_TRIGGER;
		
		getWorld()->lockPhysics();
		//log::out << "yeah " << getObjectType() << " " <<m_rigidBody->getEntity().getIndex()<< " "<<m_rigidBody->getNbColliders() << "\n";
		for (int i = 0; i < m_rigidBody->getNbColliders(); i++) {
			auto col = m_rigidBody->getCollider(i);
			col->setIsTrigger(yes);
		}
		getWorld()->unlockPhysics();
	}
	bool EngineObject::isOnlyTrigger() {
		return m_flags & ONLY_TRIGGER;
	}
	glm::mat4 EngineObject::getInterpolatedMat4(float interpolation) {
		return ToMatrix(rp3d::Transform::interpolateTransforms(prevTransform, m_rigidBody->getTransform(),interpolation));
		//glm::mat4 modelMatrix = ToMatrix(body->getTransform());
	}
	rp3d::RigidBody* EngineObject::getRigidBody() {
		return m_rigidBody;
	}
#endif
	void EngineObject::setModel(ModelAsset* asset) {
		if (m_modelAsset == asset)
			return;

		// cleanup previous model
		if (m_modelAsset) {
#ifdef ENGONE_PHYSICS
			if (m_rigidBody) {
				while (m_rigidBody->getNbColliders() != 0) {
					rp3d::Collider* col = m_rigidBody->getCollider(0);
					m_rigidBody->removeCollider(col);
				}
			}
#endif
			if (m_animator)
				m_animator->cleanup(); // cleanup instead of delete since you may still want an animator if you change the models
			m_modelAsset = nullptr;
		}

		// set new model
		m_modelAsset = asset;
		if (m_animator) {
			m_animator->asset = m_modelAsset;
		}
#ifdef ENGONE_PHYSICS
		loadColliders();
#endif
	}
	ModelAsset* EngineObject::getModel() {
		return m_modelAsset;
	}
	Animator* EngineObject::getAnimator() {
		return m_animator;
	}
	void EngineObject::removeAnimator() {
		if (m_animator) {
			ALLOC_DELETE(Animator, m_animator);
			m_animator = nullptr;
		}
	}
	Animator* EngineObject::createAnimator() {
		if (m_animator) {
			log::out << log::RED << "EngineObject : Cannot create an animator when one already exists\n";
			return m_animator;
		}
		m_animator = ALLOC_NEW(Animator)();
		m_animator->asset = m_modelAsset;
		return m_animator;
	}
	void EngineObject::setObjectType(uint32 type) {
		m_objectType = type;
	}
	uint32 EngineObject::getObjectType() {
		return m_objectType;
	}
	void EngineObject::setObjectInfo(uint32 info) {
		m_objectInfo = info;
	}
	uint32 EngineObject::getObjectInfo() {
		return m_objectInfo;
	}
	void EngineObject::setFlags(int flags) {
		m_flags = flags;
	}
	int EngineObject::getFlags() {
		return m_flags;
	}
	EngineWorld* EngineObject::getWorld() {
		return m_world;
	}
#ifdef ENGONE_PHYSICS
	void EngineObject::loadColliders() {
		if (!m_world) {
			log::out << log::RED << "EngineObject : Cannot load colliders when world is null\n";
			return;
		}
		if (!m_modelAsset) {
			log::out << log::RED << "EngineObject : Cannot load colliders when modelAsset is null\n";
			return;
		}
		if (!m_rigidBody) {
			log::out << log::RED << "EngineObject : Cannot load colliders when rigidBody is null\n";
			return;
		}
		if (!m_modelAsset->valid()) {
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
					m_world->lockCommon();
					shape = m_world->getPhysicsCommon()->createHeightFieldShape(asset->map.gridColumns,
						asset->map.gridRows, asset->map.minHeight, asset->map.maxHeight,
						asset->map.heights.data(), rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE,
						1, 1, *(rp3d::Vector3*)&scale);
					m_world->unlockCommon();
				} else if (asset->colliderType == ColliderAsset::CubeType) {
					glm::mat4 loc = transforms[i] * inst.localMat;
					rp3d::Vector3 scale = *(rp3d::Vector3*)&asset->cube.size;
					scale.x *= matScale.x;
					scale.y *= matScale.y;
					scale.z *= matScale.z;
					m_world->lockCommon();
					shape = m_world->getPhysicsCommon()->createBoxShape(scale);
					m_world->unlockCommon();
				} else if (asset->colliderType == ColliderAsset::CapsuleType) {
					float realHeight = asset->capsule.height - asset->capsule.radius * 2;
					realHeight *= matScale.y;
					float radius = asset->capsule.radius;
					radius *= (matScale.x + matScale.z) / 2;
					m_world->lockCommon();
					shape = m_world->getPhysicsCommon()->createCapsuleShape(radius, realHeight);
					m_world->unlockCommon();
				} else if (asset->colliderType == ColliderAsset::SphereType) {
					float radius = asset->sphere.radius;
					radius *= (matScale.x + matScale.y + matScale.z) / 3.f;
					m_world->lockCommon();
					shape = m_world->getPhysicsCommon()->createSphereShape(radius);
					m_world->unlockCommon();
				}
				m_world->lockPhysics();
				m_rigidBody->addCollider(shape, tr);
				auto col = m_rigidBody->getCollider(m_rigidBody->getNbColliders() - 1);
				col->getMaterial().setFrictionCoefficient(0.5f);
				col->getMaterial().setBounciness(0.0f);
				col->setUserData(m_colliderData);
				col->setIsTrigger(m_flags & ONLY_TRIGGER);
				m_world->unlockPhysics();
			}
		}
		m_flags &= ~PENDING_COLLIDERS;
		if (noColliders) {
			// updating center of mass would cause wierd stuff
			log::out << "EngineObject : Loaded no colliders\n";
		} else {
			m_world->lockPhysics();
			m_rigidBody->updateLocalCenterOfMassFromColliders();
			m_rigidBody->updateLocalInertiaTensorFromColliders();
			m_world->unlockPhysics();
		}
	}
	void EngineObject::setColliderUserData(void* data) {
		m_colliderData = data; 
		if ((m_flags & PENDING_COLLIDERS) == 0) { // colliders have been loaded so just set user data here
			m_world->lockPhysics();
			int cols = m_rigidBody->getNbColliders();
			for (int i = 0; i < cols; i++) {
				m_rigidBody->getCollider(i)->setUserData(m_colliderData);
			}
			m_world->unlockPhysics();
		} 
	}
	glm::vec3 EngineObject::getPosition() {
		m_world->lockPhysics();
		//glm::vec3 temp;
		rp3d::Vector3 temp = m_rigidBody->getTransform().getPosition();
		//*(rp3d::Vector3*)&temp = m_rigidBody->getTransform().getPosition();
		//glm::mat4 mat = getInterpolatedMat4(m_world->getApp()->getEngine()->getLoopInfo().interpolation);
		m_world->unlockPhysics();
		return *(glm::vec3*)&temp;
		//return mat[3];
	}
	void EngineObject::setPosition(const glm::vec3& position) {
		m_world->lockPhysics();
		rp3d::Transform temp = m_rigidBody->getTransform();
		temp.setPosition(*(const rp3d::Vector3*)&position);
		m_rigidBody->setTransform(temp);
		m_world->unlockPhysics();
	}
	glm::vec3 EngineObject::getLinearVelocity() {
		// m_world->lockPhysics();
		// *(rp3d::Vector3*)&temp = m_rigidBody->getLinearVelocity();
		// m_world->unlockPhysics();
		
		rp3d::Vector3 temp = m_rigidBody->getLinearVelocity();
		return *(glm::vec3*)&temp;
	}
	void EngineObject::setLinearVelocity(const glm::vec3& vec3) {
		m_world->lockPhysics();
		m_rigidBody->setLinearVelocity(*(rp3d::Vector3*)&vec3);
		m_world->unlockPhysics();
	}
	glm::vec3 EngineObject::getAngularVelocity() {
		// m_world->lockPhysics();
		// *(rp3d::Vector3*)&temp = m_rigidBody->getAngularVelocity();
		// m_world->unlockPhysics();
		// return temp;
		rp3d::Vector3 temp = m_rigidBody->getAngularVelocity();
		return *(glm::vec3*)&temp;
	}
	void  EngineObject::applyForce(const glm::vec3& force) {
		m_world->lockPhysics();
		m_rigidBody->applyWorldForceAtCenterOfMass(*(const rp3d::Vector3*)&force);
		m_world->unlockPhysics();
	}
#endif
	UUID EngineObject::getUUID() const {
		return m_uuid;
	}
}