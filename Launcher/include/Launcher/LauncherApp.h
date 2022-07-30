#pragma once

#include "Engone/Engone.h"
#include "Launcher/Settings.h"
#include "Engone/NetworkModule.h"

namespace launcher {
	enum LauncherState : uint8_t {
		LauncherConnecting=0,
		LauncherDownloading,
		LauncherSetting,
		LauncherServerSide,
	};
	class LauncherApp : public engone::Application {
	public:
		LauncherApp(engone::Engone* engone,const std::string& settings="settings.dat");
		~LauncherApp() { }

		void update(engone::UpdateInfo& info) override;
		void render(engone::RenderInfo& info) override;
		void onClose(engone::Window* window) override;

		// this will check current settings, and start server/client or error message
		void doAction(bool delay=false);

		void switchState(LauncherState newState);
		void interpretAddress();

		bool draggingWindow=false;
		float diffMX=0;
		float diffMY=0;

	private:
		engone::Window* m_window;
		Settings m_settings;
		GameCache m_cache;
		std::string root = "download\\";
		engone::Font* consolas;
		engone::Client m_client;
		engone::Server m_server;

		// UI app detail
		LauncherState state = LauncherConnecting;
		std::string launcherName = "A game launcher"; // Main text when app starts.
		float loadingTime = 0;
		engone::ui::TextBox addressText;
		engone::ui::TextBox infoText; // do not editing=true on this.

		// FileWrite
		std::unordered_map < engone::UUID, engone::FileWriter* > fileDownloads;

		std::string lastDownloadedFile;

		std::string switchInfoText;
		engone::DelayCode delaySwitch;
		engone::DelayCode delayDownloadFailed;
	};
}