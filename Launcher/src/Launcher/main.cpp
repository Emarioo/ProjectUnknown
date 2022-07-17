#include "Launcher/LauncherApp.h"

void runApp() {
	engone::Engone engone;
	launcher::LauncherApp* app = engone.createApplication<launcher::LauncherApp>("settings.dat");
	//app->address = { "host" };
	//launcher::LauncherApp* app2 = engone.createApplication<launcher::LauncherApp>("settings2.dat");
	//app2->address = { "localhost" };

	app->doAction();
	//app2->doAction();
	engone.start();

	std::cin.get();
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	runApp();
	return 0;
}
int main(int argc, char* argv[]) {
	runApp();
	std::cin.get();
	return 0;
}