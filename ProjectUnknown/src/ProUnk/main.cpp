#include "Engone/Engone.h"

#include "ProUnk/GameApp.h"

#include "Engone/Utilities/rp3d.h"

#include "Engone/Utilities/LoggingModule.h"

void runApp(int argc, char** argv) {
	using namespace engone;
	using namespace prounk;

	Engone engine;

	//-- Special options when starting the game. Like allocating a console if in Release mode.
	for (int i = 0; i < argc; i++) {
		//printf("arg %d %s\n",i,argv[i]);
		if (strcmp(argv[i], "--console") == 0 || strcmp(argv[i], "-c") == 0) {
			CreateConsole();
		}
		if (strcmp(argv[i], "--server") == 0 || strcmp(argv[i], "-s") == 0) {
			GameAppInfo info = { GameApp::START_SERVER, "1000" };
			GameApp* app = engine.createApplication<GameApp>(info);
		}
		if (strcmp(argv[i], "--client") == 0 || strcmp(argv[i], "-cl") == 0) {
			if (argc - i > 2) {

				i++;
				std::string ip = argv[i];
				i++;
				std::string port = argv[i];
				GameAppInfo info = { GameApp::START_SERVER, port, ip };
				GameApp* app = engine.createApplication<GameApp>(info);
			} else {
				log::out << log::RED << "runApp - client argument is invalid\n";
			}
		}
	}
		//Engone engine;
		//game::GameApp* app = engine.createApplication<game::GameApp>();
		//GameApp* app = engine.createApplication<GameApp>(GameApp::StartServer);
		//GameApp* app2 = engine.createApplication<GameApp>(GameApp::StartClient);

	engine.start();

	// any log message in deconstructors will not be saved. (deconstructors of global variables that is) <- what do I mean?
	log::out.getSelected().saveReport(nullptr);

	//std::cin.get();
}
// Runs the game without a console
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	//engone::CreateConsole(); // <- when debugging arguments
	int argc;
	char** argv;
	engone::ConvertArguments(argc,argv);
	runApp(argc,argv);
	engone::FreeArguments(argc,argv);
	return 0;
}
// Runs the game with a console
int main(int argc, char** argv) {
	runApp(argc,argv);
	return 0;
}