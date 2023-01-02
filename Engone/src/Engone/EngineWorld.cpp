#include "Engone/World/EngineWorld.h"
#include "Engone/Application.h"

namespace engone {

	// list CANNOT be nullptr. crash is bound to happen.
	EngineObject* EngineObjectIterator::next() {
		return m_world->getObjectAt(m_index++);
	}
	void EngineObjectIterator::restart() {
		m_index = 0;
	}
	//void EngineObjectIterator::popCurrent() {
	//	m_world->deleteObjectAt(m_index);
	//}

	EngineWorld::~EngineWorld() {
		cleanup();
	}
	EngineWorld::EngineWorld() {
	}
	EngineWorld::EngineWorld(Application* app) {
		m_app = app;
#ifdef ENGONE_PHYSICS
		m_physicsWorld = app->getPhysicsCommon()->createPhysicsWorld();
#endif
	}
	void EngineWorld::cleanup() {

		EngineObjectIterator* iterator = createIterator();
		EngineObject* obj=nullptr;
		while(obj=iterator->next()){
			//delete obj;
			ALLOC_DELETE(EngineObject, obj);
		}
		deleteIterator(iterator);
		m_objects.clear(); // should clear itself
#ifdef ENGONE_PHYSICS
		m_app->getPhysicsCommon()->destroyPhysicsWorld(m_physicsWorld);
#endif
	}
	void EngineWorld::update(LoopInfo& info) {
		m_mutex.lock();
		EngineObjectIterator* iterator = createIterator();
		EngineObject* obj;
		while(obj=iterator->next()) {
#ifdef ENGONE_PHYSICS
			if (obj->m_flags&EngineObject::PENDING_COLLIDERS) {
				obj->loadColliders(this);
			}
#endif
			if(obj->getAnimator())
				obj->getAnimator()->update(info.timeStep);
		}
		deleteIterator(iterator);
#ifdef ENGONE_PHYSICS
		if (m_physicsWorld)
			m_physicsWorld->update(info.timeStep);
#endif
		m_mutex.unlock();
	}
	EngineObject* EngineWorld::getObject(UUID uuid) {
		if (uuid == 0) return nullptr;
		EngineObjectIterator* iterator = createIterator();
		EngineObject* obj;
		while (obj = iterator->next()) {
			if (obj->getUUID() == uuid) {
				deleteIterator(iterator);
				return obj;
			}
		}
		deleteIterator(iterator);
		return nullptr;
	}
	EngineObject* EngineWorld::createObject(UUID uuid) {
		EngineObject* obj = ALLOC_NEW(EngineObject)(uuid);
		m_objects.push_back(obj);
		rp3d::Transform t;
		obj->m_rigidBody = m_physicsWorld->createRigidBody(t);
		obj->m_rigidBody->setUserData(obj);
		return obj;
	}
	class RaycastInfo : public rp3d::RaycastCallback {
	public:
		RaycastInfo() = default;

		rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& raycastInfo) override {
			using namespace engone;
			// check if item
			EngineObject* object = (EngineObject*)raycastInfo.body->getUserData();
			if (object->getObjectType() == ignoreType)
				return 1.f;
			if (object) {
				m_hitObjects.push_back(object);
			}
			m_objectLimit--;
			if (m_objectLimit>0) {
				return 1.f;
			}else{
				return 0.f;
			}
		}
		std::vector<engone::EngineObject*> m_hitObjects;
		int m_objectLimit=1;
		int ignoreType=-1;
	};
	std::vector<EngineObject*> EngineWorld::raycast(rp3d::Ray ray, int objectLimit, int ignoreObjectType) {
		RaycastInfo info;
		info.m_objectLimit = objectLimit;
		info.ignoreType = ignoreObjectType;
		getPhysicsWorld()->raycast(ray, &info);
		return std::move(info.m_hitObjects);
	}
	EngineObjectIterator* EngineWorld::createIterator() {
		EngineObjectIterator* iterator = ALLOC_NEW(EngineObjectIterator)(this);
		m_iterators.push_back(iterator);
		return iterator;
	}
	void EngineWorld::deleteIterator(EngineObjectIterator* iterator) {
		for (int i = 0; i < m_iterators.size(); i++) {
			if (m_iterators[i] == iterator) {
				ALLOC_DELETE(EngineObjectIterator, m_iterators[i]);
				m_iterators.erase(m_iterators.begin() + i);
				return;
			}
		}
	}
	// Should return nullptr if out of bounds
	EngineObject* EngineWorld::getObjectAt(int index) {
		if (index<0 || index>m_objects.size() - 1) return nullptr;
		return m_objects[index];
	}
	int EngineWorld::getObjectCount() {
		return m_objects.size();
	}
	void EngineWorld::deleteObject(EngineObject* object) {
		for (int i = 0; i < m_objects.size(); i++) {
			if (m_objects[i] == object) {
				if (object->getRigidBody()) {
					// may be bad if you do raycast while deleting rigidbody.
					getPhysicsWorld()->destroyRigidBody(object->getRigidBody());
				}
				if (object->getAnimator())
					ALLOC_DELETE(Animator,object->getAnimator());
				ALLOC_DELETE(EngineObject, object);
				//delete object;
				log::out << "EngineWorld::deleteObject - memory leak!\n";
				m_objects.erase(m_objects.begin() + i);
				for (int j = 0; j < m_iterators.size();j++) {
					auto* iter = m_iterators[j];
					if (iter->m_index>=i) {
						iter->m_index--;
					}
				}
				return;
			}
		}
	}
	void EngineWorld::deleteObjectAt(int index) {
		m_objects.erase(m_objects.begin() + index);
		for (int j = 0; j < m_iterators.size(); j++) {
			auto* iter = m_iterators[j];
			if (iter->m_index >= index) {
				iter->m_index--;
			}
		}
	}

	void EngineWorld::addParticleGroup(ParticleGroupT* group) {
		m_particleGroups.push_back(group);
	}
	rp3d::PhysicsCommon* EngineWorld::getPhysicsCommon() {
		if (!m_app) return nullptr;
		return m_app->getPhysicsCommon();
	}
	rp3d::PhysicsWorld* EngineWorld::getPhysicsWorld() {
		return m_physicsWorld;
	}
}