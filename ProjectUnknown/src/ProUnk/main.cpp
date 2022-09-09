#include "Engone/Engone.h"

#include "ProUnk/GameApp.h"

#include "Engone/Utilities/rp3d.h"

#include "Engone/Utilities/LoggingModule.h"

void runApp() {
	using namespace engone;

	{
		Engone engine;
		//game::GameApp* app = engine.createApplication<game::GameApp>();
		game::GameApp* app = engine.createApplication<game::GameApp>(game::GameApp::StartServer);
		game::GameApp* app2 = engine.createApplication<game::GameApp>(game::GameApp::StartClient);
		engine.start();
	}
	log::out.getSelected().saveReport(nullptr); // any log message in deconstructors will not be saved. (deconstructors of global variables that is)

	//std::cin.get();
}
// Runs the game without a console
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	runApp();
	return 0;
}
// Runs the game with a console
int main(int argc, char* argv[]) {
	runApp();
	return 0;
}