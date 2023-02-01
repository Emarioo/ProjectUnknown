#pragma once

#include "Engone/Engone.h"
#include "Launcher/Settings.h"
#include "Engone/Networking/NetworkModule.h"
#include "Launcher/LogLevels.h"

#include "Engone/Utilities/Locks.h"

namespace launcher {
	enum LauncherState : uint8_t {
		LauncherConnecting=0,
		LauncherDownloading,
		LauncherSetting,
		LauncherServerSide,

		LauncherClientMenu,
		LauncherServerMenu,
		//LauncherDownloading
		//LauncherConnecting
	};
	struct LauncherAppInfo {
		std::string settingsPath=SETTINGS_PATH;
		std::string cachePath=CACHE_PATH;
		LauncherState state; // values other than ClientMenu, ServerMenu will be ignored.
		std::string address;
		bool autoConnect=false;
		//std::string gameFilesPath=GAME_FILES_PATH; // not used in LauncherApp for now
	};
	class LauncherApp : public engone::Application {
	public:
		LauncherApp(engone::Engone* engone, LauncherAppInfo& info);
		~LauncherApp() override;

		void update(engone::LoopInfo& info) override;
		void render(engone::LoopInfo& info) override;
		void onClose(engone::Window* window) override;

		// this will check current settings, and start server/client or error message
		//void doAction(bool delay=false);

		// setup monitor for gamefiles
		void setupGamefiles();

		bool launchGame();

		void download();

		// client is running and not trying to connect
		bool clientFullyConnected();

		void recClientFiles(engone::MessageBuffer& buf, engone::UUID clientUUID);
		void recServerSendFile(engone::MessageBuffer& buf, engone::UUID clientUUID);
		void recServerFinished(engone::MessageBuffer& buf, engone::UUID clientUUID);
		void recServerError(engone::MessageBuffer& buf, engone::UUID clientUUID);

		// connect server or client if not connected
		void connect();

		struct FileSend {
			std::string fullPath;// where server finds the file
			std::string path; // where client should put file
			uint64_t time;
		};
		void sendFile(FileSend& file, engone::UUID clientUUID, bool exclude);

		//void switchState(LauncherState newState);
		//void interpretAddress();

		//-- Menus
		void renderClient(engone::LoopInfo& info);
		void renderDownloading(engone::LoopInfo& info);
		void renderServer(engone::LoopInfo& info);

		bool draggingWindow=false;
		float diffMX=0;
		float diffMY=0;

	private:
		engone::Window* m_window=nullptr;
		Settings m_settings;
		GameCache m_cache;
		std::string root = "download\\";
		engone::FontAsset* consolas;
		engone::Client m_client;
		engone::Server m_server;

		float downloadTime = 0;
		float topMoveEdge = 25;

		LauncherState launcherState = LauncherClientMenu;
		bool isConnecting = false;
		float connectTime = 0;
		std::string address;
		bool editingAddress=false;
		int editAddressAt = -1;
		std::string lastError;

		// downloads from when you clicked the button
		std::vector<engone::UUID> recentDownloads;
		std::mutex downloadMutex;

		//-- Style
		engone::ui::Color backgroundColor = { "001525" };
		engone::ui::Color backgroundTopColor = { "002535" };

		engone::ui::Color normalTextColor = { "E0E0E0" };
		engone::ui::Color normalTextColorHover = { "FFFFFF" };
		engone::ui::Color hiddenTextColor = { "999999" };
		engone::ui::Color hiddenTextColorHover = { "B5B5B5" };

		engone::ui::Color backBoxColor = { "002545" };
		engone::ui::Color backBoxColorHover = { "013555" };
		engone::ui::Color backBoxColorClicked = { "011040" };

		engone::ui::Color errorColor = { 1,0.05,0,1 };

		// FileWrite
		struct FileDownload {
			FileDownload() = default;
			int downloadedBytes = 0;
			int totalBytes = 0;
			std::string path; // root not included
			engone::FileWriter* writer=nullptr;
		};
		engone::Mutex fileDownloadsMutex;
		std::unordered_map< engone::UUID, FileDownload > fileDownloads;
		//std::unordered_map < engone::UUID, engone::FileWriter* > fileDownloads;

		void cleanDownloads();

		std::string switchInfoText;
		engone::DelayCode delaySwitch;
		engone::DelayCode delayDownloadFailed;

		engone::FileMonitor gameFilesRefresher;
		struct EntryInfo {
			std::string source;
			std::string destination;
			// pointer because some code complained
			//engone::FileMonitor* refresher = nullptr;
		};
		bool pendingCalculation=false;
		void calculateGameFiles();
		int gameFilesCount=0; // display purpose
		std::unordered_map<std::string, EntryInfo> gameFileEntries;
	};
}