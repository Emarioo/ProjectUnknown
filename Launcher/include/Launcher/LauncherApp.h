#pragma once

#include "Engone/Engone.h"
#include "Launcher/Settings.h"
#include "Engone/NetworkModule.h"

namespace launcher {
	class LauncherApp : public engone::Application {
	public:
		LauncherApp(engone::Engone* engone,const std::string& settings="settings.dat");

		void update(engone::UpdateInfo& info) override;
		void render(engone::RenderInfo& info) override;
		void onClose(engone::Window* window) override;

		// this will check current settings, and start server/client or error message
		void doAction();
		engone::ui::TextBox address;

		bool draggingWindow=false;
		float diffMX=0;
		float diffMY=0;
	private:
		engone::Window* m_window;
		Settings m_settings;
		GameCache m_cache;

		std::string root = "download\\";

		std::string executablePath;

		engone::Font* consolas;

		engone::Client m_client;
		engone::Server m_server;

		std::string launcherMessage;
	};
}