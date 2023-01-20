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
#ifdef ENGONE_PHYSICS
		using namespace rp3d;
		if (m_physicsCommon)
			ALLOC_DELETE(PhysicsCommon, m_physicsCommon);
		m_physicsCommon = nullptr;
#endif
	}
#ifdef ENGONE_PHYSICS
	rp3d::PhysicsCommon* Application::getPhysicsCommon() {
		return m_physicsCommon;
	}
#endif
	Window* Application::createWindow(WindowDetail detail) {
		Window* win = ALLOC_NEW(Window)(this, detail);
		GetTracker().track(win);
		m_windows.push_back(win);
		return win;
	}
	EngineWorld* Application::createWorld() {
		// Hello, bugs/crashes occur here some times. Not sure why.
		// Something with ALLOC_NEW? perhaps the compiler or visual studio intellisense?
		// It most certainly my fault in some way. Not doing stuff properly?

		// create physics common if not created
#ifdef ENGONE_PHYSICS
		if (!m_physicsCommon) {
			//m_physicsCommon = ALLOC_NEW(rp3d::PhysicsCommon)();
			m_physicsCommon = new rp3d::PhysicsCommon();// doesn't seem to like ALLOC_NEW? probably some other issue though 
			//printf("common %p\n", m_physicsCommon);
			if (!m_physicsCommon) {
				log::out << log::RED << "Application : Failed allocating PhysicsCommon!\n";
			}
		}
#endif
		//EngineWorld* world = ALLOC_NEW(EngineWorld)(this);
		EngineWorld* world = ALLOC_NEW(EngineWorld)(this);
		if (!world) {
			log::out << log::RED << "Application : Failed allocating EngineWorld!\n";
			return nullptr;
		}
#ifdef ENGONE_PHYSICS
		lockCommon();
		world->m_physicsWorld = m_physicsCommon->createPhysicsWorld();
		unlockCommon();
		//printf("world %p\n", world->m_physicsWorld);
		if (world->m_physicsWorld==nullptr) {
			log::out << log::RED << "Application : Failed creating PhysicsWorld!\n";
		}
#endif
		world->m_app = this;
		m_worlds.push_back(world);
		return world;
	}
	std::vector<EngineWorld*>& Application::getWorlds() {
		return m_worlds;
	}
	void Application::setUPS(double ups) {
		if (isMultiThreaded())
			executionTimer.aimedDelta = 1. / ups;
		else
			getEngine()->mainUpdateTimer.aimedDelta = 1. / ups;
	}
	double Application::getUPS() {
		if (isMultiThreaded())
			return 1./executionTimer.aimedDelta;
		
		return 1./getEngine()->mainUpdateTimer.aimedDelta;
	}
	double Application::getRealUPS() {
		return m_profiler.getSampler(this).getAverage();
		//if (isMultiThreaded())
		//	return 1./executionTimer.delta;
		//
		////double d = getEngine()->mainUpdateTimer.delta;
		////double f = 1. / d;
		////log::out << "d: " << d <<"  "<<f<<"\n";
		//return 1./getEngine()->mainUpdateTimer.delta;
	}
	void Application::setFPS(double fps, uint32 windowIndex) {
		getWindow(windowIndex)->setFPS(fps);
	}
	double Application::getFPS(uint32 windowIndex) {
		return getWindow(windowIndex)->getFPS();
	}
	double Application::getRealFPS(uint32 windowIndex) {
		return getWindow(windowIndex)->getRealFPS();
	}
	double Application::getRuntime() {
		if (isMultiThreaded())
			return executionTimer.runtime;

		return getEngine()->mainUpdateTimer.runtime;
	}
}