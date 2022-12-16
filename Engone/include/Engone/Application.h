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
		virtual ~Application() {
			for (int i = 0; i < m_windows.size();i++) {
				GetTracker().untrack(m_windows[i]);
				delete m_windows[i];
			}
			m_windows.clear();
			delete m_world;
		}

		// these virtual functions should be = 0 but for test purposes they are not.

		virtual void update(LoopInfo& info) {};
		virtual void render(LoopInfo& info) {};

		// will be called when a window closes.
		virtual void onClose(Window* window) {};

		// will close all windows and close the app.
		// will not delete windows from the list instantly. This is done in the game loop.
		void stop();

		inline Engone* getEngine() const { return m_engone; }

		// Will create a window which is attached to this application.
		// detail will determine what kind properties the window will have.
		// Window will be deleted when needed.
		inline Window* createWindow(WindowDetail detail = {}) {
			Window* win = new Window(this,detail);
			GetTracker().track(win);
			m_windows.push_back(win);
			return win;
		}
		inline std::vector<Window*>& getAttachedWindows() { return m_windows; }
		// Will return nullptr if index is out of bounds.
		inline Window* getWindow(uint32_t index = 0) { if (index < m_windows.size()) return m_windows[index]; return nullptr; }
		
		// the first created windows assets
		inline AssetStorage* getStorage() {
			if (m_windows.size() != 0) return m_windows[0]->getStorage();
			return nullptr;
		};
		// Return the first created windows renderer
		inline Renderer* getRenderer() {
			if (m_windows.size() != 0) return m_windows[0]->getRenderer();
			return nullptr;
		}

		// virtual because you may want your GameGround instead of the default.
		virtual inline EngineWorld* getWorld() { return m_world; }
		virtual inline void setWorld(EngineWorld* world) { m_world=world; }

		bool isRenderingWindow() const { return m_renderingWindows; }

		bool isStopped() const { return m_stopped; }

		rp3d::PhysicsCommon* getPhysicsCommon();
		
		static TrackerId trackerId;
	private:
		EngineWorld* m_world=nullptr;

		bool m_stopped = false;
		std::vector<Window*> m_windows;
		bool m_renderingWindows=false;
		Engone* m_engone=nullptr;

		rp3d::PhysicsCommon* m_pCommon = nullptr;
		
		friend class Engone;
		friend class Window;

	};
}