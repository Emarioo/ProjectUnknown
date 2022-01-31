#include "gonpch.h"

#include "Engone/Engone.h"
#include "GameHandler.h"

void runApp(bool debug) {
	engone::InitEngine();
	
	game::InitGame();

	engone::Start(game::Update, game::Render, 60);

	engone::ExitEngine();
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