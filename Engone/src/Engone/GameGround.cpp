#include "Engone/World/GameGround.h"
#include "Engone/Application.h"

namespace engone {

	//ClientPermissions::~ClientPermissions() {
	//	for (auto [id, p] : permissions) {
	//		delete p;
	//	}
	//	permissions.clear();
	//}
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
		for (int i = 0; i < m_objects.size(); i++) {
			delete m_objects[i].object;
		}
		m_objects.clear();
#ifdef ENGONE_PHYSICS
		if (m_pCommon) {
			delete m_pCommon; // should delete everything
			m_pCommon = nullptr;
			m_pWorld = nullptr;
		}
#endif
	}
	void GameGround::update(UpdateInfo& info) {
		for (int i = 0; i < m_objects.size(); i++) {
#ifdef ENGONE_PHYSICS
			if (m_objects[i].object->pendingColliders) {
				m_objects[i].object->loadColliders(this);
			}
#endif
			m_objects[i].object->update(info);
		}
#ifdef ENGONE_PHYSICS
		if (m_pWorld)
			m_pWorld->update(info.timeStep);
#endif
	}
	void GameGround::addObject(GameObject* object) {
		m_objects.push_back({ 0,object });
	}
	void GameGround::addParticleGroup(ParticleGroupT* group) {
		m_particleGroups.push_back(group);
	}
}