#include "Launcher/LauncherApp.h"

#define CONSOLE_ON_BAD CreateConsole();

void runApp(int argc, char** argv) {
	using namespace engone;
	using namespace launcher;

	Engone engone;

	//SetLogLevel(NETWORK_LEVEL, NETWORK_LEVEL_ALL & (~NETWORK_LEVEL_SPAM));
	//ConvertArguments("-server -client", argc, argv);
	
	char** originArgv = argv;
	
	// Don't replace args from console unless there are none
	if (argc == 1) {
		//ConvertArguments("-server -client", argc, argv);
	}

	LauncherAppInfo info{};
	bool writeLogReport = true;
	
	bool preventStart = false;
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {
			CreateConsole();
			preventStart = true;
			log::out
				<< log::YELLOW << "Commands:\n"
				<< log::YELLOW << " --help (will not start the launcher)\n"
				<< log::YELLOW << " --console\n"
				<< log::YELLOW << " --noreport\n"
				<< log::YELLOW << " --logreport\n"
				<< log::YELLOW << " --cachepath <path to cache>\n"
				<< log::YELLOW << " --settingspath <path to settings>\n"
				<< log::YELLOW << " --state <server OR client>\n"
				<< log::YELLOW << " --server [port] (default port is " << DEFAULT_PORT << ")\n"
				<< log::YELLOW << " --client [ip:port] (default ip:port is " << DEFAULT_IP << ":" << DEFAULT_PORT << ")\n";
			break;
		}
	}
	if (preventStart) {
		system("pause"); // find a replacement? it is here so that you can read the help commands
	}else{
		//-- Special options when starting the game. Like allocating a console if in Release mode.
		// Todo: Move this to a sub function.
		for (int i = 0; i < argc; i++) {
			if (i == 0) {
				if (originArgv == argv)
					continue; // skip path to this executable
			}
			if (strcmp(argv[i], "--console") == 0) {
				CreateConsole();
				continue;
			}
			if (strcmp(argv[i], "--noreport") == 0) {
				writeLogReport = false;
				continue;
			}
			if (strcmp(argv[i], "--logreport") == 0) {
				writeLogReport = true;
				continue;
			}
			if (strcmp(argv[i], "--cachepath") == 0) {
				if (i + 1 < argc) {
					if (argv[i + 1][0] == '-') {
						CONSOLE_ON_BAD
							log::out << log::YELLOW << "Missing path string for '" << argv[i] << "' (example: '--cachepath somewhere/launcher_cache.txt')\n";
					} else {
						i++;
						info.cachePath = argv[i];
					}
				} else {
					CONSOLE_ON_BAD
						log::out << log::YELLOW << "To few arguments for '" << argv[i] << "' (example: '--cachepath somewhere/launcher_cache.txt')\n";
				}
				continue;
			}
			if (strcmp(argv[i], "--settingspath") == 0) {
				if (i + 1 < argc) {
					if (argv[i + 1][0] == '-') {
						CONSOLE_ON_BAD
							log::out << log::YELLOW << "Missing path string for '" << argv[i] << "' (example: '--cachepath somedir/launcher_settings.txt')\n";
					} else {
						i++;
						info.settingsPath = argv[i];
					}
				} else {
					CONSOLE_ON_BAD
						log::out << log::YELLOW << "To few arguments for '" << argv[i] << "' (example: '-settingspath somedir/launcher_settings.txt')\n";
				}
				continue;
			}
			if (strcmp(argv[i], "--state") == 0) {
				if (i + 1 < argc) {
					if (argv[i + 1][0] == '-') {
						CONSOLE_ON_BAD
						log::out << log::YELLOW << "To few arguments for '" << argv[i] << "' (example: '--state server')\n";
					} else {
						i++;
						if (strcmp(argv[i], "server")) {
							info.state = LauncherServerMenu;
						} else if (strcmp(argv[i], "client")) {
							info.state = LauncherClientMenu;
						} else {
							CONSOLE_ON_BAD
							log::out << log::YELLOW << "Unknown '" << argv[i] << "' for '" << argv[i] << "' (use server OR client)\n";
						}
					}
				} else {
					CONSOLE_ON_BAD
					log::out << log::YELLOW << "To few arguments for '" << argv[i] << "' (example: '--state server')\n";
				}
				continue;
			}
			if (strcmp(argv[i], "--server") == 0) {
				std::string port;
				if (i + 1 < argc) { // port is optional
					if (argv[i + 1][0] != '-') {
						i++;
						port = argv[i];
					}
				}
				LauncherAppInfo appInfo{};
				appInfo.cachePath = info.cachePath;
				appInfo.settingsPath = info.settingsPath;
				appInfo.state = LauncherServerMenu;
				appInfo.autoConnect = true;
				if (!port.empty()) {
					appInfo.address = "127.0.0.1:" + port;
				}
				LauncherApp* app = ALLOC_NEW(LauncherApp)(&engone, appInfo);
				engone.addApplication(app);
				continue;
			}
			if (strcmp(argv[i], "--client") == 0) {
				std::string adr;
				if (i + 1 < argc) {
					if (argv[i + 1][0] != '-') {
						i++;
						adr = argv[i];
					}
				}
				LauncherAppInfo appInfo{};
				appInfo.cachePath = info.cachePath;
				appInfo.settingsPath = info.settingsPath;
				appInfo.state = LauncherClientMenu;
				appInfo.address = adr;
				appInfo.autoConnect = true;

				LauncherApp* app = ALLOC_NEW(LauncherApp)(&engone, appInfo);
				engone.addApplication(app);
				continue;
			}
			CONSOLE_ON_BAD
			log::out << log::YELLOW << "Argument '" << argv[i] << "' is unknown (see --help)\n";
		}
		if (engone.getApplications().size() == 0) {
			LauncherAppInfo info{};
			LauncherApp* app = ALLOC_NEW(LauncherApp)(&engone, info);
			engone.addApplication(app);
			//app = ALLOC_NEW(LauncherApp)(&engone, info);
			//engone.addApplication(app);

			//LauncherApp* app = ALLOC_NEW(LauncherApp)(&engone, "settings1.txt");
			//LauncherApp* app2 = ALLOC_NEW(LauncherApp)(&engone, "settings.txt");
			//engone.addApplication(app);
			//engone.addApplication(app2);
		}
		engone.start();
		if (writeLogReport) {
			log::out.getSelected().saveReport(nullptr);
		}
	}
	if (originArgv != argv) {
		FreeArguments(argc, argv);
	}
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	int argc;
	char** argv;
	engone::ConvertArguments(argc, argv);
	runApp(argc, argv);
	engone::FreeArguments(argc, argv);
	return 0;
}
int main(int argc, char** argv) {
	runApp(argc, argv);
	return 0;
}