#pragma once

#include "Engone/Window.h"
#include "Engone/AssetModule.h"
#include "Engone/LoopInfo.h"

namespace engone {

	// required in application
	class Engone;

	// maybe move somewhere else?
	class Application {
	public:
		Application(Engone* engone) : m_engone(engone) {}

		virtual void update(UpdateInfo& info) = 0;
		virtual void render(RenderInfo& info) = 0;

		// will be called when a window closes.
		virtual void onClose(Window* window) = 0;

		// will close all windows.
		void stop();

		inline Engone* getEngine() const { return m_engone; }

		// Will create a window which is attached to this application.
		// detail will determine what kind properties the window will have.
		// Window will be deleted when needed.
		inline Window* createWindow(WindowDetail detail = {}) {
			Window* win = new Window(this,detail);
			m_windows.push_back(win);
			return win;
		}
		inline std::vector<Window*>& getAttachedWindows() { return m_windows; }
		// Will return nullptr if index is out of bounds.
		inline Window* getWindow(uint32_t index = 0) { if (index < m_windows.size()) return m_windows[index]; return nullptr; }
		
		// the first created windows assets
		inline Assets* getAssets() {
			if (m_windows.size() != 0) return m_windows[0]->getAssets();
			return nullptr;
		};
		// Return the first created windows renderer
		inline Renderer* getRenderer() {
			if (m_windows.size() != 0) return m_windows[0]->getRenderer();
			return nullptr;
		}

	private:
		std::vector<Window*> m_windows;
		Engone* m_engone=nullptr;
		friend class Engone;
		friend class Window;
	};
}