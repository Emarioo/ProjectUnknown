#include "Engone/Engone.h"

#include "ProUnk/GameApp.h"

#include "Engone/Utilities/rp3d.h"

#include "Engone/Utilities/LoggingModule.h"

void runApp(int argc, char** argv) {
	using namespace engone;
	using namespace prounk;

	Engone engine;

	bool overrideArgs = false;

	//overrideArgs = true;
	//ConvertArguments("--server", argc, argv);
	ConvertArguments("--server --client 127.0.0.1 1000", argc, argv);

	//-- Special options when starting the game. Like allocating a console if in Release mode.
	for (int i = 0; i < argc; i++) {
		//printf("arg %d %s\n",i,argv[i]);
		if (strcmp(argv[i], "--console") == 0 || strcmp(argv[i], "-c") == 0) {
			CreateConsole();
		}
		//printf("HELLO %s==%s = %d\n",argv[i],"--server", strcmp(argv[i], "--server") == 0);
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
				GameAppInfo info = { GameApp::START_CLIENT, port, ip };
				GameApp* app = engine.createApplication<GameApp>(info);
			} else {
				log::out << log::RED << "runApp - client argument is invalid\n";
			}
		}
	}

	//engine.getStats().setFPSLimit(144);
	//engine.getStats().setFPSLimit(144);
	engine.start();
	if(overrideArgs)
		FreeArguments(argc, argv);

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