#include "Gamecore.h"

#include <Windows.h>

class Fruit {
public:
	Fruit() {};
	~Fruit() {};
	int count;
};

class Apple : public Fruit {
	int val;
public:
	Apple() {}
	Apple(int v) :
		val(v) {}
	int raw() { 
		return val;
	}
};
int main(int argc, char* argv[]) {
	/*for (int i = 0; i < 256;i++) {
		bug::c(i);
		bug::out + i + " color? \n";
	}*/
	if (false) {
		std::cout << "Expect buffer crash since GLFW functions hasn't been created" << std::endl;
		//MeshData* temp = new MeshData();
		//FHLoadMesh(temp,"assets/meshes/Floor");

		//CollData* temp = new CollData();
		//FHLoadColl(temp, "assets/colliders/Floor");

		AnimData* temp = new AnimData();
		fManager::LoadAnim(temp, "assets/animations/MagicStaffIdle");
		
		std::cin.get();
		return 0;
	}/*
	std::vector<Fruit> fruits;
	Apple a = AddT(Apple("red"));
	std::cout << a.color << "\n";
	*/
	gamecore::Init();
	std::cin.get();
	return 0;
}