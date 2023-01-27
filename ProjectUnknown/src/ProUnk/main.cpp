#include "Engone/Engone.h"

#include "ProUnk/GameApp.h"

#include "Engone/Utilities/rp3d.h"

#include "Engone/Utilities/LoggingModule.h"

#include "ProUnk/Tests/AllTests.h"

#include "Engone/Utilities/Alloc.h"

#include "Engone/Utilities/HashMap.h"

#include "Engone/Utilities/Stack.h"
#include "Engone/Utilities/StableArray.h"

#include "Engone/Utilities/FrameArray.h"

void runApp(int argc, char** argv) {
	using namespace engone;
	using namespace prounk;
	 
	//printf("Size: %d %d\n", sizeof(EngineObject),sizeof(std::mutex));

	//char mem[sizeof(EngineObject)];
	//
	//new(mem)EngineObject(nullptr);

	//FrameArrayTest();
	//HashMapTestCase();
	//EngoneStackTest();
	//EngoneStableArrayTest();

	//std::string eh;

	//uint8_t* raw = (uint8_t*) & eh;
	//int cap = sizeof(std::string);
	//for (int i = 0; i < cap; i++) {
	//	printf("%u\n", raw[i]);
	//}
	//memset(raw,0,cap);
	//printf("--");
	//for (int i = 0; i < cap; i++) {
	//	printf("%u\n", raw[i]);
	//}
	//std::string other="yeet";
	//eh = other;

	//std::cin.get();

	Engone engine;

	//ConvertArguments("--server", argc, argv);
	//ConvertArguments("--server 1025", argc, argv);
	//ConvertArguments("--server --client 127.0.0.1 1000 --client 127.0.0.1 1000", argc, argv);
	//ConvertArguments("--client 127.0.0.1 1000 --client 127.0.0.1 1000", argc, argv);
	
	//ConvertArguments("--server --client 127.0.0.1 --client 127.0.0.1", argc, argv);
	
	bool writeLogReport = false;
	bool preventStart = false;
	
	// help is prioritized, any other commands will be skipped
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0) {
			CreateConsole(); /// so that you can see the commands in a console
			preventStart = true;
			log::out
				<< "Commands:\n"
				<< " --help (will not start game)\n"
				<< " --console\n"
				<< " --logreport\n"
				<< " --server [port] (default port is " << Session::DEFAULT_PORT << ")\n"
				<< " --client <ip> [port] (default port is " << Session::DEFAULT_PORT << ")\n";
			break;
		}
	}

	if (preventStart) {
		system("pause"); // find a replacement? it is here so that you can read the help commands
	}else{
		//-- Special options when starting the game. Like allocating a console if in Release mode.
		for (int i = 0; i < argc; i++) {
			if (strcmp(argv[i], "--console") == 0) {
				CreateConsole();
				continue;
			}
			if (strcmp(argv[i], "--logreport") == 0) {
				writeLogReport = true;
				continue;
			}
			if (strcmp(argv[i], "--server") == 0) {
				std::string port = Session::DEFAULT_PORT;
				if (i + 1 < argc) { // port is optional
					if (argv[i + 1][0] != '-') {
						i++;
						port = argv[i];
					}
				}
				GameAppInfo info = { GameApp::START_SERVER, port, "127.0.0.1" };

				GameApp* app = ALLOC_NEW(GameApp)(&engine, info);
				engine.addApplication(app);
				continue;
			}
			if (strcmp(argv[i], "--client") == 0) {
				if (i + 1 < argc) {
					if (argv[i + 1][0] == '-') {
						log::out << log::YELLOW << "To few arguments for '" << argv[i] << "' (example: '--client 192.168.1.66')\n";
					} else {
						i++;
						std::string ip = argv[i];
						std::string port = Session::DEFAULT_PORT;
						if (i + 1 < argc) { // port is optional
							if (argv[i + 1][0] != '-') {
								i++;
								port = argv[i];
							}
						}

						GameAppInfo info = { GameApp::START_CLIENT, port, ip };
						GameApp* app = ALLOC_NEW(GameApp)(&engine, info);
						engine.addApplication(app);
					}
				} else {
					log::out << log::YELLOW << "To few arguments for '" << argv[i] << "' (example: '--client 192.168.1.66')\n";
				}
				continue;
			}

			log::out << log::YELLOW << "Argument '" << argv[i] << "' is unknown (see --help)\n";
		}
		if (engine.getApplications().size() == 0) {
			GameAppInfo info = { 0, Session::DEFAULT_PORT, "127.0.0.1" };
			GameApp* app = ALLOC_NEW(GameApp)(&engine, info);
			engine.addApplication(app);
		}
		engine.start();
		if (writeLogReport) {
			// any log message in deconstructors will not be saved. (deconstructors of global variables that is)
			log::out.getSelected().saveReport(nullptr);
		}
	}
	//FreeArguments(argc, argv);

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
	using namespace engone;
	//class Apple {
	//public:
	//	Apple(float sour, float size) : sour(sour), size(size) {}
	//	~Apple() {}
	//	float sour, size;
	//};


	//Apple* eh = NEW(Apple)(2, 5);

	//DELETE(Apple, eh);

	//eh->~Apple();
	//alloc::free(eh, 8);

	//std::cin.get();
	runApp(argc,argv);
	return 0;
}