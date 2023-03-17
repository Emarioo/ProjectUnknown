#include "Engone/Engone.h"

// #include "ProUnk/GameApp.h"

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
	// using namespace prounk;
	Engone engone;

    AppInstance* app = engone.createApp();
#ifndef LIVE_EDITING
    app->preUpdateProc = GamePreUpdate;
    app->postUpdateProc = GamePostUpdate;
	GameInit(&engone,app,0);
#else
	// app->libraryId = engone.addLibrary("D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode\\Debug-MSVC\\GameCode.dll",
	// 	"D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode\\Debug-MSVC\\GameCode.pdb");
	app->libraryId = engone.addLibrary("D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode.dll",
		"D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode.pdb");
#endif
    engone.start();
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