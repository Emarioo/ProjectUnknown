
#include "ProUnk/UI/UIMenus.h"

#include "ProUnk/GameApp.h"

namespace prounk {
	void TestScene(GameApp* app) {
		using namespace engone;

		//pipeShader = new Shader(pipeGLSL);
		/*HeapMemory memory;

		struct A {
			float x, y;
		};
		struct B {
			std::string name;
			int age;
		};
		struct C {
			int data[20];
		};

		A a[3]{ {1,2},{3,4},{5,6} };
		B b[3]{ {"George",5},{"Tom",54},{"Sara",19}};
		C c[3];
		memory.writeMemory<A>('A', a);
		memory.writeMemory<B>('B', b);
		memory.writeMemory<B>('B', b+1);
		memory.writeMemory<C>('C', c);
		memory.writeMemory<A>('A', a+1);


		while(true) {
			char type = memory.readType();
			if(type=='A') {
				A* ptr = memory.readItem<A>();
				log::out << "A: " << ptr->x << " " << ptr->y<< "\n";
			} else if (type == 'B') {
				B* ptr = memory.readItem<B>();
				log::out << "B: "<<ptr->name << " " << ptr->age << "\n";
			} else if (type == 'C') {
				C* ptr = memory.readItem<C>();

				log::out << "C: " << ptr->data[0] << " " << ptr->data[1] << "\n";
			} else if (type == 0)
				break;
		}*/


		//bug::out < a->Get(0)->Get(0)->frames[1].value < bug::end;
		//engine::AddAnimationAsset("ArmatureAction");
		//engine::GetModelAsset("Player")->AddAnimation("ArmatureAction");

		//engine::AddTextureAsset("brickwall");
		//engine::AddTextureAsset("brickwall_normal");

		//engine::AddTextureAsset("items0");
		//engine::AddTextureAsset("magicstaff_fireball");
		{
			// Remove some of the quaternion values and see if it works

			/*AddAnimationAsset("Quater");
			quater = GetAnimationAsset("Quater");

			quater->Modify(0, 60);

			FCurve x,y,z,w;

			z.Add({ Bezier, 0, 0 });
			z.Add({ Bezier, 60, 0.5 });

			//cha.Add(PosX, x);
			channel.Add(QuaX, z);
			//cha.Add(QuaZ, z);
			//cha.Add(QuaW, w);

			quater->AddObjectChannels(0, channel);
			*/
			//std::cout << "obs "<< quater->objects.size() << std::endl;
			//std::cout << "channs "<< quater->objects[0].fcurves.size() << std::endl;
		}

		/*Tree* tree = new Tree();
		AddEntity(tree);*/

		/*
		ModelObject* tree = new ModelObject(9, 0, 0, engone::GetAsset<ModelAsset>("Oak/Oak"));
		AddObject(tree);
		*/

		/*	ModelObject* terrain = new ModelObject(0, 0, 0, engone::GetAsset<ModelAsset>("Terrain/Terrain"));
			AddObject(terrain);
			*/

			//Player* player = new Player();
			//m_player = new Player();
			//AddEntity(m_player);
			//AddSystem(m_player);

			//Goblin* goblin = new Goblin();
			//AddEntity(goblin);
			//AddSystem(goblin);

			//goblin->getComponent<Transform>()->position.y = 6;

			//player->getComponent<MeshRenderer>()->renderMesh = false;

			/*ColliderAsset* as = new ColliderAsset("Terrain/Plane.003");
			as->Load(as->filePath);*/

			//as->Load("assets/Terrain/Terrain.collider");

			/*Entity* terrain = new Entity(ComponentEnum::Transform|ComponentEnum::Model|ComponentEnum::Physics);
			AddEntity(terrain);
			terrain->getComponent<Model>()->modelAsset = GetAsset<ModelAsset>("Terrain/Terrain");
			terrain->getComponent<Model>()->renderMesh = false;
			terrain->getComponent<Physics>()->renderCollision = true;*/

			//log::out << "tree ptr " << (void*)tree->getComponent<Model>() << "\n";

			//EntityIterator iterator = GetEntityIterator(ComponentEnum::Model);
			//while (iterator) {
			//	//Transform* transform = iterator.get<Transform>();
			//	//Physics* physics = iterator.get<Physics>();
			//	Model* modelC = iterator.get<Model>();

			//	log::out << modelC->modelAsset->baseName << "\n";
			//}

			//game::SetPlayer(player);

			//DirLight* l = new DirLight({ 2,-4,1 });
			//AddLight(l);

			//float fArray[]{
			//	0,0,1, 0,1,1, 0,0,0,
			//	0,0,1, 0,1,1, 0,0,0,
			//	0,0,1, 0,1,1, 0,0,0,
			//};
			//uint32_t iArray[]{
			//	0,1,2
			//};

			//VBO.setData(sizeof(fArray) / sizeof(float), fArray);
			//IBO.setData(3, iArray);

			//VAO.addAttribute(2);
			//VAO.addAttribute(4, &VBO);
	}
}