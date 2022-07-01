#pragma once

#include "Engone/Window.h"

namespace engone {
	// Will automatically add itself to engone. No need to AddApplication(...)
	class Application {
	public:

		Application(float fps=60.0f);

		virtual void update(float delta) = 0;
		virtual void render() = 0;

		// will be called when 'window' closes.
		virtual void onClose(Window* window) = 0;

		// You do not need to call delete on a window that has been attached
		void attachWindow(Window* window);
		std::vector<Window*>& getAttachedWindows();
	private:
		float m_fps=60.0f;
		std::vector<Window*> m_windows;

		friend void Start();
	};
}