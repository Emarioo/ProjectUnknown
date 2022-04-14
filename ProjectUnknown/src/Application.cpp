
#include "Engone/Engone.h"
#include "GameHandler.h"

void runApp(bool debug) {

	/*engone::Window window(engone::WindowMode::Windowed);
	window.setTitle("ProjectUnknown");

	engone::InitEngone(engone::EngoneHint::Game3D|engone::EngoneHint::UI);

	game::InitGame();

	engone::Start(game::Update, game::Render, 60);*/
}
// Runs the game without a console
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	runApp(false);
	return 0;
}
// Runs the game with a console
int main(int argc, char* argv[]) {
	runApp(true);
	return 0;
}