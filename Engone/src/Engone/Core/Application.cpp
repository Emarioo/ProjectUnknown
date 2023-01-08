#include "Engone/Engone.h"
#include "Engone/Application.h"

namespace engone {
	TrackerId Application::trackerId="Application";
	void Application::stop() {
		m_stopped = true;
		for (int i = 0; i < m_windows.size(); i++) {
			m_windows[i]->close();
		}
	}
	void Application::cleanup() {
		for (int i = 0; i < m_windows.size(); i++) {
			GetTracker().untrack(m_windows[i]);
			//delete m_windows[i];
			ALLOC_DELETE(Window, m_windows[i]);
		}
		m_windows.clear();
		for (int i = 0; i < m_worlds.size(); i++) {
			ALLOC_DELETE(EngineWorld,m_worlds[i]);
			//delete m_worlds[i];
		}
		m_worlds.clear();
		using namespace rp3d;
		if (m_physicsCommon)
			ALLOC_DELETE(PhysicsCommon, m_physicsCommon);
		m_physicsCommon = nullptr;
	}
	rp3d::PhysicsCommon* Application::getPhysicsCommon() {
		return m_physicsCommon;
	}
	Window* Application::createWindow(WindowDetail detail) {
		Window* win = ALLOC_NEW(Window)(this, detail);
		GetTracker().track(win);
		m_windows.push_back(win);
		return win;
	}
	EngineWorld* Application::createWorld() {
		// create physics common if not created
		if (!m_physicsCommon) {
			m_physicsCommon = ALLOC_NEW(rp3d::PhysicsCommon)();
			if (!m_physicsCommon) {
				log::out << log::RED << "Application : Failed allocating PhysicsCommon!\n";
			}
		}
		EngineWorld* world = ALLOC_NEW(EngineWorld)(this);
		if (!world) {
			log::out << log::RED << "Application : Failed allocating EngineWorld!\n";
			return nullptr;
		}
		world->m_physicsWorld = m_physicsCommon->createPhysicsWorld();
		if (world->m_physicsWorld==nullptr) {
			log::out << log::RED << "Application : Failed creating PhysicsWorld!\n";
		}
		world->m_app = this;
		m_worlds.push_back(world);
		return world;
	}
	std::vector<EngineWorld*>& Application::getWorlds() {
		return m_worlds;
	}
}