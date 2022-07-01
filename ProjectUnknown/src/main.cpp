
#include "Engone/Engone.h"
#include "Engone/ScriptingModule.h"

#include "GameApp.h"

void runApp(bool debug) {

	game::GameApp app;

	engone::Initialize();

	engone::Start();

	engone::Cleanup();
}
// Runs the game without a console
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	runApp(false);
	return 0;
}
class A {
public:
	A() = default;
	void hej() {}
};
// Runs the game with a console
int main(int argc, char* argv[]) {
	runApp(true);
	//std::cout << "HERE\n";



	//using namespace engone;

	//InitScripts();

	//Script* script = new Script("script.js");
	//std::cin.get();
	//while (true) {
	//	script->run();

	//	std::cin.get();
	//	script->reload();
	//}
	//UninitScripts();

	return 0;
}