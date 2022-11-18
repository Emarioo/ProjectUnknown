#include "Engone/World/EngineWorld.h"
#include "Engone/Application.h"

namespace engone {

	// list CANNOT be nullptr. crash is bound to happen.
	EngineObject* EngineObjectIterator::next() {
		return m_world->getObjectAt(m_index++);
	}
	void EngineObjectIterator::popCurrent() {
		m_world->deleteObjectAt(m_index);
	}

	EngineWorld::~EngineWorld() {
		cleanup();
	}
	EngineWorld::EngineWorld() {
#ifdef ENGONE_PHYSICS
		//if (!noPhysics) {
		m_pCommon = new rp3d::PhysicsCommon();
		m_pWorld = m_pCommon->createPhysicsWorld();
		//}
#endif
	}
	void EngineWorld::cleanup() {

		// 
		//m_mutex.lock();
		//EngineObjectIterator iterator = getIterator();
		//EngineObject* obj;
		//while(obj=iterator.next()){
		//	delete obj;
		//}
		//m_objects.clear(); // should clear itself
		//m_mutex.unlock();
#ifdef ENGONE_PHYSICS
		if (m_pCommon) {
			delete m_pCommon; // should delete everything
			m_pCommon = nullptr;
			m_pWorld = nullptr;
		}
#endif
	}
	void EngineWorld::update(LoopInfo& info) {
		//m_mutex.lock();
		EngineObjectIterator iterator = getIterator();
		EngineObject* obj;
		while(obj=iterator.next()) {
#ifdef ENGONE_PHYSICS
			if (obj->flags&EngineObject::PENDING_COLLIDERS) {
				obj->loadColliders(this);
			}
#endif
			obj->update(info);
			obj->animator.update(info.timeStep);
		}
#ifdef ENGONE_PHYSICS
		if (m_pWorld)
			m_pWorld->update(info.timeStep);
#endif
		//m_mutex.unlock();
	}
	EngineObject* EngineWorld::getObject(UUID uuid) {
		if (uuid == 0) return nullptr;
		EngineObjectIterator iterator = getIterator();
		EngineObject* obj;
		while (obj = iterator.next()) {
			if (obj->getUUID() == uuid) {
				return obj;
				break;
			}
		}
		return nullptr;
	}

	void EngineWorld::addObject(EngineObject* object) {
		m_objects.push_back(object);
	}

	EngineObjectIterator EngineWorld::getIterator() {
		return EngineObjectIterator(this);
	}
	// Should return nullptr if out of bounds
	EngineObject* EngineWorld::getObjectAt(int index) {
		if (index<0 || index>m_objects.size() - 1) return nullptr;
		return m_objects[index];
	}
	void EngineWorld::deleteObject(EngineObject* object) {
		for (int i = 0; i < m_objects.size(); i++) {
			if (m_objects[i] == object) {
				m_objects.erase(m_objects.begin() + i);
				return;
			}
		}
	}
	void EngineWorld::deleteObjectAt(int index) {
		m_objects.erase(m_objects.begin() + index);
	}

	void EngineWorld::addParticleGroup(ParticleGroupT* group) {
		m_particleGroups.push_back(group);
	}
}