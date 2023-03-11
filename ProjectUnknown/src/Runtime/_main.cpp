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

#include "ProUnk/Game.h"

#include "Engone/PlatformModule/GameMemory.h"
#ifdef WIN32
#include "Engone/Win32Includes.h"
#endif

void runApp(int argc, char** argv) {
	using namespace engone;
	using namespace prounk;

	Engone engine;

    AppInstance appInstance;
    appInstance.initProc = GameInit;
    appInstance.updateProc = GameUpdate;
    engine.add(appInstance);
		
    engine.start();
}
#ifdef WIN32
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
#endif
// Runs the game with a console
int main(int argc, char** argv) {
	runApp(argc,argv);
	return 0;
}