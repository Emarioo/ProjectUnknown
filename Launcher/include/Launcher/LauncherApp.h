#pragma once

#include "Engone/Engone.h"
#include "Launcher/Settings.h"
#include "Engone/Networking/NetworkModule.h"
#include "Launcher/LogLevels.h"

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

		void update(engone::LoopInfo& info) override;
		void render(engone::LoopInfo& info) override;
		void onClose(engone::Window* window) override;

		// this will check current settings, and start server/client or error message
		void doAction(bool delay=false);

		bool launchGame();

		struct FileSend {
			std::string fullPath;// where server finds the file
			std::string path; // where client should put file
			uint64_t time;
		};
		void sendFile(FileSend& file, engone::UUID clientUUID, bool exclude);

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
		engone::FontAsset* consolas;
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

		engone::FileMonitor gameFilesRefresher;
		struct EntryInfo {
			std::string source;
			std::string destination;
			// pointer because some code complained
			engone::FileMonitor* refresher;
		};
		std::unordered_map<std::string, EntryInfo> gameFileEntries;
	};
}