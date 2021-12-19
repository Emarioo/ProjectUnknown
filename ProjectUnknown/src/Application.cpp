#include "gonpch.h"

#include "Engone/Engone.h"
#include "GameHandler.h"

void runApp() {
	engone::InitEngone();
	
	game::InitGame();

	engone::Start(game::Update, game::Render, 60);
}
// Runs the game without a console
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	runApp();
	return 0;
}
// Runs the game with a console
int main(int argc, char* argv[]) {
	runApp();
	return 0;
}