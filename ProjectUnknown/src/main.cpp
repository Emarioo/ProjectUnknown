#include "Engone/Engone.h"

#include "GameApp.h"

#include "Engone/Utilities/rp3d.h"

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

	glm::mat4 mat = glm::mat4(1.0, 0.0, 0.0, 0.0,
		0.0, 0.9396926164627075, 0.3420201241970062, 0.0,
		0.0, -0.3420201241970062, 0.9396926164627075, 0.0,
		0.0, 0.0, 0.0, 1.0);

	//glm::mat4 mat = glm::mat4(1.0, 0.0, 0.0, 0.8796985149383545,
	//	0.0, 0.9396926164627075, -0.3420201241970062, 4.725200653076172,
	//	0.0, 0.3420201241970062, 0.9396926164627075, 0.42754626274108887,
	//		0.0, 0.0, 0.0, 1.0);
	glm::mat4 mi = glm::rotate(20.f * glm::pi<float>() / 180.f, glm::vec3(1, 0, 0));
	//log::out << mi << "\n";

	glm::quat rot;
	DecomposeGlm(mi,nullptr,&rot,nullptr);

	glm::vec3 eul = glm::eulerAngles(rot);
	//log::out << "eul " << eul << "\n";

	glm::mat4 fin = glm::rotate(eul.x, glm::vec3(1, 0, 0));

	//log::out << fin << "\n";


	engone::Engone engine;
	game::GameApp* app = engine.createApplication<game::GameApp>();
	engine.start();

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