#include "Launcher/LauncherApp.h"

void runApp(int argc, char** argv) {
	using namespace engone;
	using namespace launcher;

	Engone engone;

	//SetLogLevel(NETWORK_LEVEL, NETWORK_LEVEL_ALL & (~NETWORK_LEVEL_SPAM));
	//ConvertArguments("--server --client 127.0.0.1 1000", argc, argv);

	//-- Special options when starting the game. Like allocating a console if in Release mode.
	//for (int i = 0; i < argc; i++) {
	//	if (strcmp(argv[i], "--console") == 0 || strcmp(argv[i], "-c") == 0) {
	//		CreateConsole();
	//	}
	//	if (strcmp(argv[i], "--server") == 0 || strcmp(argv[i], "-s") == 0) {
	//		//GameAppInfo info = { GameApp::START_SERVER, "1000" };

	//		LauncherApp* app = ALLOC_NEW(LauncherApp)(&engone, "settings1.dat");
	//		engone.addApplication(app);
	//	}
	//	if (strcmp(argv[i], "--client") == 0 || strcmp(argv[i], "-cl") == 0) {
	//		if (argc - i > 2) {
	//			i++;
	//			std::string ip = argv[i];
	//			i++;
	//			std::string port = argv[i];
	//			GameAppInfo info = { GameApp::START_CLIENT, port, ip };
	//			GameApp* app = ALLOC_NEW(GameApp)(&engine, info);
	//			engine.addApplication(app);
	//		} else {
	//			log::out << log::RED << "runApp - client argument is invalid\n";
	//		}
	//	}
	//}
	if (engone.getApplications().size() == 0) {
		LauncherApp* app = ALLOC_NEW(LauncherApp)(&engone, "settings1.dat");
		LauncherApp* app2 = ALLOC_NEW(LauncherApp)(&engone, "settings.dat");
		engone.addApplication(app);
		engone.addApplication(app2);
	}

	engone.start();

	log::out.getSelected().saveReport(nullptr);
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