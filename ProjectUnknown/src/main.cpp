#include "Engone/Engone.h"

#include "GameApp.h"

void runApp() {
	using namespace engone;

	//std::string str = std::filesystem::current_path().generic_string();
	//std::cout << "hello "<<str << "\n";
	//double time = 1.001;
	//while (true) {
		//time = 0.001 + 5*time;
		//std::string str = FormatTime(363.123456,false,255);
		//log::out <<" "<< str << "\n";
		//std::this_thread::sleep_for(std::chrono::milliseconds(700));
	//}

	engone::Engone engine;
	game::GameApp* app = engine.createApplication<game::GameApp>();
	engine.start();

	std::cin.get();
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