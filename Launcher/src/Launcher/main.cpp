#include "Launcher/LauncherApp.h"

void runApp() {
	using namespace engone;

	Engone engone;
	launcher::LauncherApp* app = engone.createApplication<launcher::LauncherApp>("settings1.dat");
	launcher::LauncherApp* app2 = engone.createApplication<launcher::LauncherApp>("settings.dat");

	engone.start();

	//std::cin.get();
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	runApp();
	return 0;
}
int main(int argc, char* argv[]) {
	runApp();
	return 0;
}