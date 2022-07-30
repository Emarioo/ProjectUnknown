#include "Engone/Engone.h"
#include "Engone/Application.h"

namespace engone {
	TrackerId Application::trackerId="Application";
	void Application::stop() {
		for (int i = 0; i < m_windows.size(); i++) {
			m_windows[i]->close();
		}
	}
}