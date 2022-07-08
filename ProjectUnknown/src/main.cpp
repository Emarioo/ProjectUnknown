
#include "Engone/Engone.h"
#include "Engone/ScriptingModule.h"

#include "GameApp.h"

#include "reactphysics3d/reactphysics3d.h"

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
void phystest() {
	using namespace reactphysics3d;

	PhysicsCommon common;

	PhysicsWorld* world = common.createPhysicsWorld();

	Vector3 pos(0, 20, 0);
	Quaternion rot = Quaternion::identity();
	Transform trans(pos, rot);
	RigidBody* body = world->createRigidBody(trans);

	const decimal timeStep = 1.0f / 60.0f;

	for (int i = 0; i < 20; i++) {
		world->update(timeStep);

		const Transform& transform = body->getTransform();
		const Vector3& position = transform.getPosition();

		std::cout << "Body pos: (" << position.x << ", " << position.y << ", " << position.z << ")\n";
	}

	std::cin.get();
}
// Runs the game with a console
int main(int argc, char* argv[]) {
	phystest();
	//runApp(true);
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