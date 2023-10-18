#include "Engone/Engone.h"

#include "ProUnk/Game.h"
#include "Engone/PlatformModule/GameMemory.h"

void runApp(int argc, char** argv) {
	using namespace engone;
	
	Engone* engone = Engone::InitializeGlobal();

    AppInstance* app = engone->createApp();
#ifndef LIVE_EDITING
    app->preUpdateProc = GamePreUpdate;
    app->postUpdateProc = GamePostUpdate;
	GameInit(engone,app,0);
#else
	// app->libraryId = engone.addLibrary("D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode\\Debug-MSVC\\GameCode.dll",
	// 	"D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode\\Debug-MSVC\\GameCode.pdb");
	app->libraryId = engone->addLibrary("bin\\GameCode.dll",
		"bin\\GameCode.pdb");
#endif
    engone->start();
}
#ifdef OS_WINDOWS
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