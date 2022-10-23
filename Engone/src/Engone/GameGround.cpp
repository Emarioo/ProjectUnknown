#include "Engone/World/GameGround.h"
#include "Engone/Application.h"

namespace engone {
	GameGround::~GameGround() {
		cleanup();
	}
	GameGround::GameGround(Application* app) : m_app(app) {
#ifdef ENGONE_PHYSICS
		//if (!noPhysics) {
		m_pCommon = new rp3d::PhysicsCommon();
		m_pWorld = m_pCommon->createPhysicsWorld();
		//}
#endif
	}
	void GameGround::cleanup() {
		m_mutex.lock();
		for (int i = 0; i < m_objects.size(); i++) {
			delete m_objects[i];
		}
		m_objects.clear();
		m_mutex.unlock();
#ifdef ENGONE_PHYSICS
		if (m_pCommon) {
			delete m_pCommon; // should delete everything
			m_pCommon = nullptr;
			m_pWorld = nullptr;
		}
#endif
	}
	void GameGround::update(UpdateInfo& info) {
		m_mutex.lock();
		for (int i = 0; i < m_objects.size(); i++) {
#ifdef ENGONE_PHYSICS
			if (m_objects[i]->flags&GameObject::PENDING_COLLIDERS) {
				m_objects[i]->loadColliders(this);
			}
#endif
			m_objects[i]->update(info);
		}
#ifdef ENGONE_PHYSICS
		if (m_pWorld)
			m_pWorld->update(info.timeStep);
#endif
		m_mutex.unlock();
	}
	void GameGround::addObject(GameObject* object) {
		m_mutex.lock();
		m_objects.push_back(object);
		m_mutex.unlock();
	}
	void GameGround::addParticleGroup(ParticleGroupT* group) {
		m_particleGroups.push_back(group);
	}
}