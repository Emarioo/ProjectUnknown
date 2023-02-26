#include "Gamecore.h"

#include <Windows.h>

#include <functional>

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
template<typename T, size_t N>
void ReadBytes(std::ifstream* f, T* d) {
	f->read(reinterpret_cast<char*>(&(d[0])), sizeof(T) * N);
}
int main(int argc, char* argv[]) {

	
	if (false) {
		std::cout << "Expect buffer crash since GLFW functions hasn't been created" << std::endl;
		//MeshData* mesh = new MeshData();
		//mesh->hasError = true;
		//fManager::LoadMesh(mesh,"goblin_body");
		BoneData* bone = new BoneData();
		bone->hasError = true;
		fManager::LoadBone(bone, "goblin_skeleton");
		//fManager::LoadMesh(temp,"VertexColor");

		//CollData* temp = new CollData();
		//FHLoadColl(temp, "assets/colliders/Floor");

		//AnimData* temp = new AnimData();
		//fManager::LoadAnim(temp, "assets/animations/MagicStaffIdle");
		
		std::cin.get();
		return 0;
	}

	gamecore::Init();
	std::cin.get();
	return 0;
}