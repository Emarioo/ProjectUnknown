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
	//ConvertArguments("--server --client 127.0.0.1 1000 --client 127.0.0.1 1000", argc, argv);
	//ConvertArguments("--client 127.0.0.1 1000 --client 127.0.0.1 1000", argc, argv);
	
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
			
			GameApp* app = ALLOC_NEW(GameApp)(&engine, info);
			engine.addApplication(app);
		}
		if (strcmp(argv[i], "--client") == 0 || strcmp(argv[i], "-cl") == 0) {
			if (argc - i > 2) {
				i++;
				std::string ip = argv[i];
				i++;
				std::string port = argv[i];
				GameAppInfo info = { GameApp::START_CLIENT, port, ip };
				GameApp* app = ALLOC_NEW(GameApp)(&engine, info);
				engine.addApplication(app);
			} else {
				log::out << log::RED << "runApp - client argument is invalid\n";
			}
		}
	}
	if (engine.getApplications().size() == 0) {
		GameAppInfo info = { 0, "1000", "127.0.0.1"};
		GameApp* app = ALLOC_NEW(GameApp)(&engine, info);
		engine.addApplication(app);
	}

	engine.getStats().setFPSLimit(60);
	engine.getStats().setUPSLimit(60);
	engine.setFlags(EngoneEnableDebugInfo);
	engine.start();
	//FreeArguments(argc, argv);

	// any log message in deconstructors will not be saved. (deconstructors of global variables that is)
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