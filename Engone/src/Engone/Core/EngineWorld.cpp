#include "Engone/World/EngineWorld.h"
#include "Engone/Application.h"

namespace engone {

	EngineObjectIterator::EngineObjectIterator(EngineWorld* world) {
		m_world = world;
	}
	EngineObject* EngineObjectIterator::next() {
		finish(); // release access to previous object

		m_world->m_objectsMutex.lock();
		bool running = m_world->m_objects.iterate(m_iterator);

		if (!running) {
			m_world->m_objectsMutex.unlock();
			return nullptr;
		}
		UUID uuid = m_iterator.ptr->getUUID();
		m_world->m_objectsMutex.unlock();

		EngineObject* obj = m_world->requestAccess(uuid);
		if (!obj) {
			m_iterator.ptr = nullptr;
		}
		return obj;
	}
	void EngineObjectIterator::restart() {
		finish();
		m_iterator = {};
	}
	void EngineObjectIterator::finish() {
		if (m_iterator.ptr) {
			m_world->releaseAccess(m_iterator.ptr->getUUID());
		}
	}
	EngineWorld::~EngineWorld() {
		cleanup();
	}
	EngineWorld::EngineWorld(Application* app) {
		m_app = app;
//#ifdef ENGONE_PHYSICS
//		m_physicsWorld = app->getPhysicsCommon()->createPhysicsWorld();
//#endif
	}
	void EngineWorld::cleanup() {


		EngineObjectIterator iterator = createIterator();
		EngineObject* obj=nullptr;
		while(obj=iterator.next()){
			deleteObject(obj->getUUID());

			//delete obj;
			//ALLOC_DELETE(EngineObject, obj);
		}
		//deleteIterator(iterator);

		//m_iteratorsMutex.lock();
		//std::vector<EngineObjectIterator*> iterators = std::move(m_iterators);
		//m_iteratorsMutex.unlock();

		//for (int i = 0; i < iterators.size(); i++) {
		//	EngineObjectIterator* iterator = iterators[i];
		//	iterator->finish();
		//	ALLOC_DELETE(EngineObjectIterator, m_iterators[i]);
		//}
	
		//m_objects.clear(); // should clear itself

#ifdef ENGONE_PHYSICS
		m_app->getPhysicsCommon()->destroyPhysicsWorld(m_physicsWorld);
#endif
	}
	void EngineWorld::update(LoopInfo& info) {
		uint32_t index = 0;

		EngineObjectIterator iterator(this);
		EngineObject* obj=nullptr;
		while (obj=iterator.next()) {
			if (obj->m_flags & EngineObject::PENDING_COLLIDERS) {
				obj->loadColliders();
			}
			if (obj->getAnimator())
				obj->getAnimator()->update(info.timeStep);
		}

#ifdef ENGONE_PHYSICS
		if (m_physicsWorld)
			m_physicsWorld->update(info.timeStep);
#endif
	}
	//EngineObject* EngineWorld::getObject(UUID uuid) {
	//	if (uuid == 0) return nullptr;
	//	m_objectsMutex.lock();
	//	
	//	EngineObject* obj=nullptr;
	//	for (int i = 0; i < m_objects.size();i++) {
	//		
	//		if (m_objects[i]->getUUID() == uuid) {
	//			obj = m_objects[i];
	//			break;
	//		}
	//	}

	//	m_objectsMutex.unlock();
	//	return obj;
	//}
	EngineObject* EngineWorld::createObject(UUID uuid) {
		m_objectsMutex.lock();
		uint32_t index = m_objects.add({});
		EngineObject* obj = m_objects.get(index);
		obj->init(this, uuid);
		obj->m_objectIndex = index;
		uuid = obj->getUUID();

		log::out << "Created Object " << uuid<<"\n";
		// No thread has access to object yet because they need to use the get method but can only do that with the objectsMutex which is locked.

		obj->m_mutex.lock(); // Will never wait
		m_objectsMutex.unlock();

		m_objectMapMutex.lock();
		m_objectMap[uuid].object=obj;
		m_objectMapMutex.unlock();

		return obj;
	}
	EngineObject* EngineWorld::requestAccess(UUID uuid) {
		m_objectMapMutex.lock();
		auto find = m_objectMap.find(uuid);
		if (find == m_objectMap.end()) {
			m_objectMapMutex.unlock();
			return nullptr;
		}
		if (find->second.pendingDelete) {
			m_objectMapMutex.unlock();
			return nullptr;
		}

		EngineObject* obj = find->second.object; // object will never be nullptr
		find->second.waitingThreads++;

		m_objectMapMutex.unlock();

		obj->m_mutex.lock();

		m_objectMapMutex.lock();
		// find may become invalid due to changes to the objectMap
		auto find2 = m_objectMap.find(uuid);
		if (find2 != m_objectMap.end()) {
			find2->second.waitingThreads--;
		}
		m_objectMapMutex.unlock();

		return obj;
	}
	//void EngineWorld::releaseAccess(EngineObject* obj) {
	//	releaseAccess(obj->getUUID());
	//}
	void EngineWorld::releaseAccess(UUID uuid) {
		m_objectMapMutex.lock();
		auto find = m_objectMap.find(uuid);
		bool valid = find != m_objectMap.end();
		
		if (find == m_objectMap.end()) {
			m_objectMapMutex.unlock();
			return;
		}
		EngineObject* obj = find->second.object;
		
		obj->m_mutex.unlock();
		
		// delete object
		if (!find->second.pendingDelete || find->second.waitingThreads != 0) {
			m_objectMapMutex.unlock();
			return;
		}
		m_objectMap.erase(uuid);
		m_objectMapMutex.unlock();

		m_objectsMutex.lock();
		m_objects.remove(obj->m_objectIndex);
		log::out << "Deleted Object " << uuid << "\n";
		m_objectsMutex.unlock();
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
	EngineObjectIterator EngineWorld::createIterator() {
		return { this };
		//EngineObjectIterator* iterator = ALLOC_NEW(EngineObjectIterator)(this);
		//m_iteratorsMutex.lock();
		//m_iterators.push_back(iterator);
		//m_iteratorsMutex.unlock();
		//return iterator;
	}
	//void EngineWorld::deleteIterator(EngineObjectIterator* iterator) {
	//	m_iteratorsMutex.lock();
	//	for (int i = 0; i < m_iterators.size(); i++) {
	//		if (m_iterators[i] == iterator) {
	//			iterator->finish();
	//			ALLOC_DELETE(EngineObjectIterator, m_iterators[i]);
	//			m_iterators.erase(m_iterators.begin() + i);
	//			break;
	//		}
	//	}
	//	m_iteratorsMutex.unlock();
	//}
	void EngineWorld::deleteObject(UUID uuid) {
		m_objectMapMutex.lock();
		auto find = m_objectMap.find(uuid);
		if (find == m_objectMap.end()) {
			m_objectMapMutex.unlock();
			return;
		}
		EngineObject* obj = find->second.object;
		find->second.pendingDelete = true;
		m_objectMapMutex.unlock();
		
		releaseAccess(uuid);
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