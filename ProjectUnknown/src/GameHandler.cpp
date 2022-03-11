#include "gonpch.h"

#include "Engone/EventHandler.h"
#include "Engone/Engone.h"

#include "GameHandler.h"
#include "UI/InterfaceManager.h"
#include "Keybindings.h"

static const char* experimentGLSL = {
#include "Shaders/experiment.glsl"
};
static const char* depthGLSL = {
#include "Shaders/depth.glsl"
};

#include "Objects/Goblin.h"
#include "Objects/Gnorg.h"
#include "Objects/Tutorial.h"
#include "Objects/Player.h"
#include "Objects/ModelObject.h"
#include "Objects/Tree.h"

#include "Engone/Rendering/Buffer.h"

#include "Engone/Handlers/ObjectHandler.h"

#include "Engone/UI/UIPipeline.h"

namespace game
{
	static Player* player=nullptr;
	Player* GetPlayer() {
		return player;
	}
	void SetPlayer(Player* _player) {
		player = _player;
	}
	engone::EventType OnKey(engone::Event& e)
	{
		if (engone::CheckState(GameState::RenderGame)) {
			if (e.action == 1) {
				if (e.key == GLFW_KEY_O) {
					/*engone::ReadOptions();
					if (engone::GetDimension() != nullptr)
						engone::GetDimension()->UpdateAllChunks();*/
				}
				if (e.key == GLFW_KEY_G) {// what in the world does this mean ->   AT: Magic Editor
					/*IElement* el = GetElement("editor_script");
					if (el != nullptr) {
						if (!el->selected) {

							if (HasTag(8)) {
								//DelTag(8);

								renderer::SetCursorMode(false);
								// Pause game?
							} else {
								//AddTag(8);
								renderer::SetCursorMode(true);
								// Other things?
							}

						}
					}*/
				}
				if (engone::IsKeybindingDown(KeyPause)) {
					if (engone::CheckState(GameState::Paused)) {
						engone::SetState(GameState::Paused, false);
						engone::LockCursor(true);
					}
					else {
						engone::SetState(GameState::Paused, true);
						engone::LockCursor(false);
					}
				}
			}
		}
		return engone::EventType::None;
	}

	//  Do something about that const string. It's not supposed to be there.
	engone::EventType OnMouse(engone::Event& e)
	{
		//bug::out < elementName < (elementName=="uiFade")<bug::end;
		/*if (IsGameState(Play)) {
			if (action == 0) {
				if (engine::GetPauseMode()) { // Unpause in pause menu if clicking on anything
					engine::SetPauseMode(false);
				}
			}
		}*/
		return engone::EventType::None;
	}
	void Update(double delta) {
		using namespace engone;

		//melody.UpdateStream();

		//light->position.x = 5 * glm::cos(engone::GetPlayTime());// bez3 * 2;
		//light->position.z = 5 * glm::sin(engone::GetPlayTime());// bez3 * 2;

		UpdateEngine(delta);

		Player* player = GetPlayer();
		Transform* t = player->getComponent<Transform>();
		if (player->thirdPerson) {
			glm::mat4 mat = glm::translate(t->position + glm::vec3(0, 3.f, 0)) * glm::rotate(GetCamera()->rotation.y, glm::vec3(0, 1, 0)) * glm::rotate(GetCamera()->rotation.x, glm::vec3(1, 0, 0)) * glm::translate(glm::vec3(0, 0, 5));
			GetCamera()->position = mat[3];
		} else {
			GetCamera()->position = t->position+glm::vec3(0,3.f,0);
		}

	}

	static bool initGameAssets = false;
	void InitGame() {
		using namespace engone;

		LockCursor(true);
		if (!initGameAssets) {
			InitItemHandler();
			//interfaceManager.SetupGameUI();
			initGameAssets = true;
		}

		if (false) {
			SetState(GameState::RenderGame, false);
			SetState(GameState::Intro, true);
			//ui::SetupIntro();

			IElement* cover = new IElement("darkCover", 9999);
			cover->CenterX(0)->CenterY(0)->Width(1.f)->Height(1.f)->Color({ 0.f });
			cover->add("fade").Fade({ 0.f,0.f }, .5f);

			IElement* intro = new IElement("introTexture");
			intro->Left(0)->CenterY(0)->Width(1.f)->Height(1.f)->Fixed(800 / 669.f)->Color({ 1.f })->Texture(GetAsset<Texture>("textures/intro"));
			IElement* blank = new IElement("introBlank", 9990);
			blank->CenterX(0)->CenterY(0)->Width(1.f)->Height(1.f)->Color({ 1.f });
			blank->add(intro);

			AddTimer(2.f, [=] {
				cover->setTransition("fade", true);
				AddTimer(.5 + 2.f, [=] {
					cover->setTransition("fade", false);
					AddTimer(.5f, [=] {
						RemoveElement(blank);
						cover->setTransition("fade", true);
						SetState(GameState::RenderGame, true);
						SetState(GameState::Intro, false);
						LockCursor(true);
						});
					});
				});

			AddElement(blank);
			AddElement(cover);
		}

		//-- Assets
		AddAsset<Shader>("depth", new Shader(depthGLSL));
		AddAsset<Shader>("experiment", new Shader(experimentGLSL));
		AddAsset<Font>("consolas", "fonts/consolas42");

		//-- Event and game loop functions
		AddListener(new Listener(EventType::Key, OnKey));
		AddListener(new Listener(EventType::Click, OnMouse));

		if (LoadKeybindings("data/keybindings.dat") < KEY_COUNT) {
			CreateDefualtKeybindings();
		}

		TestScene();
	}
	engone::VertexBuffer VBO;
	engone::VertexBuffer VBO2;
	engone::IndexBuffer IBO;
	engone::VertexArray VAO;

	//engone::TriangleBuffer* TBO;

	void uitest() {
		using namespace engone;
		//Box box = {100,100,400,200, 1, 0.2f, 1, 1};
		//Box* box2 = ui::Rect({ 150.f, 150.f, 100.f, 100.f, 0.5f, 0.f, 0.f, 1.f });

		/*for (int i = 0; i < 10000;i++) {
			ui::Rect({ 10 + 5.f*(i%100),10+5.f*(i/100),4.f,4.f ,1.f,1.f,1.f,1.f});
		}*/
		Box* box = ui::Rect({ 100.f, 100.f, 100.f, 100.f, 1.f, 0.f, 0.f, 1.f });

		if (ui::Clicked(*box)) {
			log::out << "Yay\n";
		}
	}

	static float rot = 0;
	static float frame = 0;
	static float move = 0;
	void Render(double lag) {
		using namespace engone;

		//ModelAsset* model = GetAsset<ModelAsset>("Snake/Snake");
		//Shader* shader = GetAsset<Shader>("armature");
		//shader->Bind();

		//glm::mat4 fin = glm::mat4(1);

		//glm::mat4 axis = glm::mat4(1);

		//axis[1][1] = 0;
		//axis[2][1] = 1;
		//axis[1][2] = -1;
		//axis[2][2] = 0;

		//glm::vec3 pos = glm::vec3(1), euler, scale;
		//glm::mat4 quater = glm::mat4(1);


		//AnimationAsset* anim = GetAsset<AnimationAsset>("Snake/SnakeAnim");
		//frame += anim->defaultSpeed * 1.f / 60;

		//if (frame > 60) {
		//	frame = 0;
		//}

		//short usedChannels = 0;
		//anim->objects[0].GetValues(frame, 1,
		//	pos, euler, scale, quater, &usedChannels);

		//glm::mat4 gameObject = glm::mat4(1);

		//AssetInstance& aInst = model->instances[0];
		//AssetInstance& mInst = model->instances[1];
		//ArmatureAsset* armature = aInst.asset->cast<ArmatureAsset>();
		//MeshAsset* mesh = mInst.asset->cast<MeshAsset>();
		//
		//glm::mat4 inve = glm::inverse((mInst.localMat));
		//
		//glm::mat4 bone = armature->bones[0].localMat*quater*armature->bones[0].invModel*mInst.localMat;// *(armature->bones[0].localMat * quater);

		//shader->SetMatrix("uBoneTransforms[0]", bone);
		//shader->SetMatrix("uTransform",gameObject* axis*aInst.localMat);

		//PassMaterial(shader, 0, mesh->materials[0]);
		//mesh->buffer.Draw();
	
		uitest();

		RenderEngine(lag);

		Shader* shad = GetAsset<Shader>("object");
		shad->bind();

		//MeshAsset* as = GetAsset<MeshAsset>("Player/Stick-N");
		move += 1/60.f/20.f;

		float fi[]{
			move / 4,-move/2, 0,
			-move / 4,-move, 0,
		};
		//as->instanceBuffer.setData(6, fi);
		////as->vertexArray->selectBuffer(1, as->instanceBuffer);
		//as->vertexArray.draw(&as->indexBuffer, 2);

		float fa[]{
			-move/2+0.f,0,
			-move/6 + 0.15f,move,
			move*2 + 0.1f,0.25
		};
		//VBO2.setData(6, fa);
		//VAO.selectBuffer(1, &VBO2);
		//VAO.draw(&IBO,3);

		//shad->SetVec4("uColor", 1, 1, 0, 1);

		//TBO->Draw();

		// Render custom things like items dragged by the mouse
		//ui::Render(2);
	}
	void TestScene()
	{
		using namespace engone;

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

		//Tree* tree = new Tree();
		//AddEntity(tree);

		//Tree* tree = new Tree();
		//AddEntity(tree);
		//for (int i = 0; i < 10000;i++) {

		//	//log::out << i <<" "<<(void*)tree->stackPtr<< "\n";
		//	
		//	
		//	tree->getComponent<Transform>()->position = {GetRandom()*1000.f,GetRandom()*2,GetRandom()*1000};
		//}

		/*for (int i = 0; i < entities.size(); i++) {
			Transform* t = entities.getEntityComponent<Transform>(i);
			log::out << t->position << "\n";
		}*/
		/*
		foreach(entity, entities) {
			log::out << entity->entityId <<" "<<entity->entityClass << "\n";
			log::out << entity->getComponent<Transform>()->position << "\n";
		}*/

		/*
		ModelObject* tree = new ModelObject(9, 0, 0, engone::GetAsset<ModelAsset>("Oak/Oak"));
		AddObject(tree);
		*/
		
	/*	ModelObject* terrain = new ModelObject(0, 0, 0, engone::GetAsset<ModelAsset>("Terrain/Terrain"));
		AddObject(terrain);
		*/
		//log::out << "tree ptr " << (void*)tree->getComponent<Model>() << "\n";
		Player* player = new Player();
		AddEntity(player);
		AddSystem(player);
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

		game::SetPlayer(player);

		DirLight* l = new DirLight({ 2,-4,1 });
		AddLight(l);

		float fArray[]{
			0,0,1,0,1,1,
			.1,0,0,1,1,1,
			.1,.1,1,1,0,1
		};
		uint32_t iArray[]{
			0,1,2
		};
		
		VBO.setData(sizeof(fArray)/sizeof(float),fArray);
		IBO.setData(3,iArray);

		VAO.addAttribute(2);
		VAO.addAttribute(4, &VBO);
		VAO.addAttribute(2, 1, &VBO2);
	}
}