#include "Engone/Engone.h"

#include "ProUnk/GameApp.h"

// #include "Engone/Utilities/rp3d.h"

// #include "Engone/Logger.h"

// #include "ProUnk/Tests/AllTests.h"

// #include "Engone/Utilities/Alloc.h"

// #include "Engone/Structures/HashMap.h"
// #include "Engone/Structures/Stack.h"
// #include "Engone/Structures/StableArray.h"
// #include "Engone/Structures/FrameArray.h"

#include "Engone/PlatformModule/GameMemory.h"

void runApp(int argc, char** argv) {
	using namespace engone;
	using namespace prounk;
	
	// log::out << "Broken?\n";
	
	 //GameMemoryTest();

	//FrameArrayTest();
	//HashMapTestCase();
	//EngoneStackTest();
	//EngoneStableArrayTest();
	
	 //system("pause");

	 //return;
	 //auto tp = MeasureSeconds();
	
	// char* ptr = (char*)Allocate(2e9);
	
	// auto sec = StopMeasure(tp);
	// log::out << "Time: "<<sec<<" ptr: "<<ptr<<"\n";
	// tp = MeasureSeconds();
	
	// ptr = (char*)Reallocate(ptr,2e9,2e9+4000);
	
	// sec = StopMeasure(tp);
	// log::out << "Time2: "<<sec<<" ptr: "<<ptr<<"\n";

	// return;
	// FileReader reader("");

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

	char** originArgv = argv;
	// Don't replace args from console unless there are none
	if (argc == 1) {
		
		//ConvertArguments("--server", argc, argv);
		//ConvertArguments("--server 1025", argc, argv);
		//ConvertArguments("--server --client 127.0.0.1 1000 --client 127.0.0.1 1000", argc, argv);
		//ConvertArguments("--client 127.0.0.1 1000 --client 127.0.0.1 1000", argc, argv);
		// 
		 //ConvertArguments("--server --client", argc, argv);
	}
	
	bool writeLogReport = true;
	bool preventStart = false;
	
	// help is prioritized, any other commands will be skipped
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			CreateConsole(); /// so that you can see the commands in a console
			preventStart = true;
			log::out
				<< log::YELLOW << "Commands:\n"
				<< log::YELLOW << " --help (will not start game)\n"
				<< log::YELLOW << " --console\n"
				<< log::YELLOW << " --noreport\n"
				<< log::YELLOW << " --logreport\n"
				<< log::YELLOW << " --server [port] (default port is " << Session::DEFAULT_PORT << ")\n"
				<< log::YELLOW << " --client [ip:port] (default ip:port is " << Session::DEFAULT_IP<<":"<< Session::DEFAULT_PORT << ")\n";
			break;
		}
	}

	if (preventStart) {
		system("pause"); // find a replacement? it is here so that you can read the help commands
	}else{
		//-- Special options when starting the game. Like allocating a console if in Release mode.
		for (int i = 0; i < argc; i++) {
			if (i == 0) {
				if (originArgv == argv)
					continue; // skip path to this executable
			}
			if (strcmp(argv[i], "--console") == 0) {
				CreateConsole();
				continue;
			}
			if (strcmp(argv[i], "-noreport") == 0) {
				writeLogReport = false;
				continue;
			}
			if (strcmp(argv[i], "--logreport") == 0) {
				writeLogReport = true;
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
				GameAppInfo info = { GameApp::START_SERVER};
				if (!port.empty()) {
					info.address = "127.0.0.1:" + port;
				}

				GameApp* app = ALLOC_NEW(GameApp)(&engine, info);
				engine.addApplication(app);
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
				GameAppInfo info = { GameApp::START_CLIENT, adr };

				GameApp* app = ALLOC_NEW(GameApp)(&engine, info);
				engine.addApplication(app);
				continue;
			}
			CreateConsole(); // user won't see the text without console
			log::out << log::YELLOW << "Argument '" << argv[i] << "' is unknown (see --help)\n";
		}
		if (engine.getApplications().size() == 0) {
			GameAppInfo info = { 0, "" };
			GameApp* app = ALLOC_NEW(GameApp)(&engine, info);
			engine.addApplication(app);
		}
		engine.start();
		if (writeLogReport) {
			// any log message in deconstructors will not be saved. (deconstructors of global variables that is)
			//log::out.getSelected().saveReport(nullptr);
		}
	}
	if (originArgv != argv) {
		FreeArguments(argc, argv);
	}
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
	//Free(eh, 8);

	//std::cin.get();
	runApp(argc,argv);
	return 0;
}