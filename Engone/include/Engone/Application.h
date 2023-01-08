#pragma once

#include "Engone/Window.h"
#include "Engone/AssetModule.h"
#include "Engone/LoopInfo.h"
#include "Engone/Utilities/RuntimeStats.h"
#include "Engone/World/EngineWorld.h"

namespace engone {

	// required in application
	class Engone;

	// maybe move somewhere else?
	class Application {
	public:
		Application(Engone* engone) : m_engone(engone) {}
		~Application() {
			cleanup();
		}
		// frees allocated memory that the application is responsible for.
		void cleanup();

		// these virtual functions should be = 0 but for test purposes they are not.

		virtual void update(LoopInfo& info) {};
		virtual void render(LoopInfo& info) {};

		// will be called when a window closes.
		virtual void onClose(Window* window) {};

		// will close all windows and close the app.
		// will not delete windows from the list instantly. This is done in the game loop.
		// function is somewhat asynchronous in this sense.
		void stop();

		inline Engone* getEngine() const { return m_engone; }

		// Will create a window which is attached to this application.
		// detail will determine what kind properties the window will have.
		// Window will be deleted when needed.
		Window* createWindow(WindowDetail detail = {});
		inline std::vector<Window*>& getAttachedWindows() { return m_windows; }
		// Will return nullptr if index is out of bounds.
		inline Window* getWindow(uint32_t index = 0) { if (index < m_windows.size()) return m_windows[index]; return nullptr; }
		
		// the first created windows assets
		inline AssetStorage* getStorage() {
			if (m_windows.size() != 0) return m_windows[0]->getStorage();
			return nullptr;
		};
		// Return the first created windows renderer
		//inline Renderer* getRenderer() {
		//	if (m_windows.size() != 0) return m_windows[0]->getRenderer();
		//	return nullptr;
		//}

		EngineWorld* createWorld();
		std::vector<EngineWorld*>& getWorlds();

		bool isRenderingWindow() const { return m_renderingWindows; }

		bool isStopped() const { return m_stopped; }

		rp3d::PhysicsCommon* getPhysicsCommon();
		
		static TrackerId trackerId;
	private:
		//EngineWorld* m_world=nullptr;

		bool m_stopped = false;
		std::vector<Window*> m_windows;
		bool m_renderingWindows=false;
		Engone* m_engone=nullptr;

		std::vector<EngineWorld*> m_worlds;

		rp3d::PhysicsCommon* m_physicsCommon = nullptr;
		
		friend class Engone;
		friend class Window;

	};
}