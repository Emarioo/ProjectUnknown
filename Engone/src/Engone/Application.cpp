#include "Engone/Engone.h"
#include "Engone/Application.h"

namespace engone {
	Application::Application(float fps) : m_fps(fps) {
		AddApplication(this);
	}
	void Application::attachWindow(Window* window) {
		m_windows.push_back(window);
	}
	std::vector<Window*>& Application::getAttachedWindows() {
		return m_windows;
	}
}