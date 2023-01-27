#include "Launcher/LauncherApp.h"

void runApp(int argc, char** argv) {
	using namespace engone;
	using namespace launcher;

	Engone engone;

	//SetLogLevel(NETWORK_LEVEL, NETWORK_LEVEL_ALL & (~NETWORK_LEVEL_SPAM));
	//ConvertArguments("--server --client 127.0.0.1 1000", argc, argv);
	
	LauncherAppInfo info{};
	bool writeLogReport = false;
	
	bool preventStart = false;
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {
			CreateConsole();
			preventStart = true;
			log::out
				<< "Commands:\n"
				<< " --help (will not start the launcher)\n"
				<< " --console\n"
				<< " --logreport\n"
				<< " --cachepath <path to cache>\n"
				<< " --settingspath <path to settings>\n"
				<< " --state <'server' OR 'client'> (without apostrophes)\n";
			break;
		}
	}
	if (preventStart) {
		system("pause"); // find a replacement? it is here so that you can read the help commands
	}else{
		//-- Special options when starting the game. Like allocating a console if in Release mode.
		// Todo: Move this to a sub function.
		for (int i = 0; i < argc; i++) {
			if (strcmp(argv[i], "--console") == 0) {
				CreateConsole();
				continue;
			}
			if (strcmp(argv[i], "--logreport") == 0) {
				writeLogReport = true;
				continue;
			}
			if (strcmp(argv[i], "--cachepath") == 0) {
				if (i + 1 < argc) {
					if (argv[i + 1][0] == '-') {
						log::out << log::YELLOW << "Missing path string for '" << argv[i] << "' (example: '--cachepath somewhere/launcher_cache.txt')\n";
					} else {
						i++;
						info.cachePath = argv[i];
					}
				} else {
					log::out << log::YELLOW << "To few arguments for '" << argv[i] << "' (example: '--cachepath somewhere/launcher_cache.txt')\n";
				}
				continue;
			}
			if (strcmp(argv[i], "--settingspath") == 0) {
				if (i + 1 < argc) {
					if (argv[i + 1][0] == '-') {
						log::out << log::YELLOW << "Missing path string for '" << argv[i] << "' (example: '--cachepath somedir/launcher_settings.txt')\n";
					} else {
						i++;
						info.settingsPath = argv[i];
					}
				} else {
					log::out << log::YELLOW << "To few arguments for '" << argv[i] << "' (example: '--settingspath somedir/launcher_settings.txt')\n";
				}
				continue;
			}
			if (strcmp(argv[i], "--state") == 0) {
				if (i + 1 < argc) {
					if (argv[i + 1][0] == '-') {
						log::out << log::YELLOW << "To few arguments for '" << argv[i] << "' (example: '--state server')\n";
					} else {
						i++;
						if (strcmp(argv[i], "server")) {
							info.state = LauncherServerMenu;
						} else if (strcmp(argv[i], "client")) {
							info.state = LauncherClientMenu;
						} else {
							log::out << log::YELLOW << "Unknown '" << argv[i] << "' for '" << argv[i] << "' (use server OR client)\n";
						}
					}
				} else {
					log::out << log::YELLOW << "To few arguments for '" << argv[i] << "' (example: '--state server')\n";
				}
				continue;
			}

			log::out << log::YELLOW << "Argument '" << argv[i] << "' is unknown (see --help)\n";
			//if (strcmp(argv[i], "--server") == 0 || strcmp(argv[i], "-s") == 0) {
			//	//GameAppInfo info = { GameApp::START_SERVER, "1000" };

			//	LauncherApp* app = ALLOC_NEW(LauncherApp)(&engone, "settings1.dat");
			//	engone.addApplication(app);
			//}
			//if (strcmp(argv[i], "--client") == 0 || strcmp(argv[i], "-cl") == 0) {
			//	if (argc - i > 2) {
			//		i++;
			//		std::string ip = argv[i];
			//		i++;
			//		std::string port = argv[i];
			//		GameAppInfo info = { GameApp::START_CLIENT, port, ip };
			//		GameApp* app = ALLOC_NEW(GameApp)(&engine, info);
			//		engine.addApplication(app);
			//	} else {
			//		log::out << log::RED << "runApp - client argument is invalid\n";
			//	}
			//}
		}
		if (engone.getApplications().size() == 0) {
			LauncherAppInfo info{};
			LauncherApp* app = ALLOC_NEW(LauncherApp)(&engone, info);
			engone.addApplication(app);
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
	//std::cin.get();
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