
#include "GameApp.h"
#include "Engone/EventModule.h"

#include "Keybindings.h"
#include "Objects/Player.h"
#include "Objects/Sword.h"
#include "Objects/Dummy.h"

#include "Magic/Focals.h"

#include "CombatData.h"

#include "Engone/Tests/BasicRendering.h"

namespace game {

	//-- Shaders
	//static const char* experimentGLSL = {
	//#include "Shaders/experiment.glsl"
	//};
	static const char* particleGLSL = {
#include "Engone/Shaders/particle.glsl"
//#include "Engone/Tests/testParticle.glsl"
	};
	//static const char* depthGLSL = {
	//#include "Shaders/depth.glsl"
	//};
	//static const char* testGLSL = {
	//#include "Shaders/test.glsl"
	//};
	static const char* blurGLSL = {
#include "Engone/Shaders/blur.glsl"
	};
	//void GameApp::onTrigger(const rp3d::CollisionCallback::CallbackData& callbackData) {
	//	using namespace engone;
	//	//log::out << "COMEON \n";
	//	for (int pairI = 0; pairI < callbackData.getNbContactPairs(); pairI++) {
	//		auto pair = callbackData.getContactPair(pairI);
	//		void* ptr1 = pair.getCollider1()->getUserData();
	//		void* ptr2 = pair.getCollider2()->getUserData();
	//		if (!ptr1 || !ptr2) continue;
	//		UserData* userData1 = (UserData*)ptr1;
	//		UserData* userData2 = (UserData*)ptr2;
	//		log::out << userData1 << " " << userData2 << "\n";
	//		if (userData1->type != userData2->type) {
	//			if (userData1->type == AttackData::TYPE && userData2->type == DefenseData::TYPE) {
	//				engone::log::out << "WOW\n";
	//				DefenseData* atk = (DefenseData*)userData2;
	//				rp3d::CollisionBody* defBody = pair.getBody2();
	//				rp3d::Vector3 medNorm{};
	//				rp3d::Vector3 medPoint{};
	//				for (int j = 0; j < pair.getNbContactPoints(); j++) {
	//					auto point = pair.getContactPoint(j);
	//					medNorm += point.getWorldNormal();
	//					medPoint += point.getLocalPointOnCollider2();
	//				}
	//				medNorm /= (float)pair.getNbContactPoints();
	//				medPoint /= (float)pair.getNbContactPoints();
	//				// for now, defense can be assumed to be a rigidbody
	//				((rp3d::RigidBody*)defBody)->applyWorldForceAtLocalPosition(medNorm, medPoint);
	//			}
	//		}
	//	}
	//}
	void GameApp::onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) {
		using namespace engone;
		//log::out << "COMEON \n";
		for (int pairI = 0; pairI < callbackData.getNbOverlappingPairs(); pairI++) {
			auto pair = callbackData.getOverlappingPair(pairI);

			void* ptr1 = pair.getCollider1()->getUserData();
			void* ptr2 = pair.getCollider2()->getUserData();
			if (!ptr1 || !ptr2) continue;

			struct Data {
				Data() : user(nullptr), body(nullptr) {}
				Data(void* user, rp3d::CollisionBody* body) : user((UserData*)user), body(body) {}
				UserData* user = nullptr;
				rp3d::CollisionBody* body = nullptr;
			};
			Data data1 = {ptr1,pair.getBody1()};
			Data data2 = {ptr2,pair.getBody2()};
			if (data1.user->type != data2.user->type) {
				Data atk = data1;
				Data def = data2;
				if (data1.user->type == DefenseData::TYPE)
					def = data1;
				if (data2.user->type == AttackData::TYPE)
					atk = data2;

				AttackData* atkData = (AttackData*)atk.user;
				DefenseData* defData = (DefenseData*)def.user;

				//log::out << "cool: "<<defData->hitCooldown << "\n";

				if (!atkData->attacking) continue;
				if (defData->hitCooldown!=0) continue;

				defData->hitCooldown = atkData->animationTime;
				// hit cooldown is determined by the attacker's animation time left
					
				// NOTE: if the attack anim. is restarted when the attack collider still is in the defense collider the cooldown would still be active
				//	Meaning no damage.
				//rp3d::CollisionBody* atkBody = pair.getBody1();
				//rp3d::CollisionBody* defBody = pair.getBody2();

				rp3d::Vector3 force = def.body->getTransform().getPosition() - atk.body->getTransform().getPosition();
				//force.y = 1;
				force *= 60.f*6.f;
				//log::out << force << "\n";
				 //for now, defense can be assumed to be a rigidbody

				((rp3d::RigidBody*)def.body)->applyWorldForceAtCenterOfMass(force);
			}
		}
	}
	engone::EventType OnKey(engone::Event& e);
	engone::EventType OnMouse(engone::Event& e);
	GameApp::GameApp(engone::Engone* engone) : Application(engone) {
		using namespace engone;
		
		m_window = createWindow({ WindowedMode,1000,800 });

		//m_window = new Window(this,1000,800);
		m_window->setTitle("Project Unknown");
		m_window->enableFirstPerson(true);

		Shader* shaderPart = new Shader(particleGLSL);
		particleGroup = new ParticleGroup();
		particleGroup->init(m_window, shaderPart);
		//engone->getParticleModule().init(m_window);

		Assets* assets = getAssets();

		Shader* blur = assets->set("blur", new Shader(blurGLSL));
		particleGroup->createCube({ 5,0,0 }, {5,5,5 }, 100000, {0,0,0});
		//particleGroup->createCube({ 5,0,0 }, { 0.2,0.2,0.2 }, 100000, {0,0,5});
		//Particle* part = particleGroup->createParticles(1);
		//part->pos = { 0,0,5 };
		//part->vel = { 3,0,0 };
		engone->addParticleGroup(particleGroup);
		//part = new Shader(partGlsl);
		//assets->set("particle", part);
		//group.init(m_window,part);
		//group.resize(1000);
		//Particle* parts = group.createParticles(1000);
		//for (int i = 0; i < 1000; i++) {
		//	parts[i].pos = { i / 20,0,0 };
		//}

		//float arr[6 * 100];
		//memset(arr, 0, sizeof(arr));
		////Particle* parts = group.createParticles(1000);
		//for (int i = 0; i < sizeof(arr) / 6 / 4; i++) {
		//	arr[6 * i] = i / 20.f;
		//}
		//part->bind();
		//buffer.bind();
		//buffer.bindBase(0);
		//buffer.setData(sizeof(arr), arr);


		//-- Assets
		//assets->set("depth")
		//AddAsset<Shader>("depth", new Shader(depthGLSL));
		//AddAsset<Shader>("experiment", new Shader(experimentGLSL));
		//AddAsset<Shader>("test", new Shader(testGLSL));
		assets->set<Font>("consolas", "fonts/consolas42");

		//-- Event and game loop functions
		m_window->attachListener(EventKey, OnKey);
		m_window->attachListener(EventClick, OnMouse);

		// ISSUE: if default keybindings change but keybindings.dat exists then the new keybinds won't apply.
		//		Temp. fix by always creating default bindings.
		//if (LoadKeybindings("data/keybindings.dat") < KEY_COUNT) {
			CreateDefualtKeybindings();
		//}

		engone->m_pCommon = new rp3d::PhysicsCommon(); // needs to be moved inside engine
		engone->m_pWorld = engone->m_pCommon->createPhysicsWorld();

		engone->m_pWorld->setIsDebugRenderingEnabled(true);
		rp3d::DebugRenderer& debugRenderer = engone->m_pWorld->getDebugRenderer();
		debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_POINT, true);

		getEngine()->m_pWorld->setEventListener(this);

		//sword = new Sword(engone);
		//sword->setTransform({ 2,0,0 });
		//engone->addObject(sword);

		player = new Player(engone);
		player->setTransform({ 0,0,0 });
		player->inventorySword = sword;
		engone->addObject(player);

		//part = new Shader(partGlsl);
		//assets->set("particle",part);
		////group.init(m_window,part);
		////group.resize(1000);
		////Particle* parts = group.createParticles(1000);
		////for (int i = 0; i < 1000; i++) {
		////	parts[i].pos = { i / 20,0,0 };
		////}

		//float arr[6 * 100];
		//memset(arr, 0, sizeof(arr));
		////Particle* parts = group.createParticles(1000);
		//for (int i = 0; i < sizeof(arr)/6/4; i++) {
		//	arr[6*i] = i/20;
		//}
		//part->bind();
		//buffer.bind();
		//buffer.bindBase(0);
		//buffer.setData(sizeof(arr), arr);


		Dummy* dummy = new Dummy(engone);
		dummy->setTransform({ 0,0,9 });
		dummy->rigidBody->setLinearVelocity({ 9,0,0 });
		engone->addObject(dummy);

		//rp3d::Vector3 anchor(1, 1, 1);
		//rp3d::FixedJointInfo fixedInfo(player->rigidBody,sword->rigidBody,anchor);
		//rp3d::Joint* joint = engone->m_pWorld->createJoint(fixedInfo);

		//m_window->getRenderer()->getCamera()->setPosition(1,1,2);
		//m_window->getRenderer()->getCamera()->setRotation(0,-0.5f,0);

		//terrain = new Terrain(engone);
		//engone->addObject(terrain);
		//terrain->setTransform({ 0,-2,0 });

		DirLight* dir = new DirLight({ -0.2,-1,-0.2 });
		engone->addLight(dir);
	}
	//float wantX=500;
	//float x = 100;
	//float velX = 0;
	void GameApp::update(engone::UpdateInfo& info) {
		using namespace engone;
		if (engone::IsKeybindingPressed(KeyPause)) {
			if(engone::GetActiveWindow()->isCursorLocked()){
				engone::GetActiveWindow()->lockCursor(false);
			} else {
				engone::GetActiveWindow()->lockCursor(true);
			}
		}
		//float t = info.timeStep;
		//float vx = (wantX - x)/t;
		//vx *= 0.5;
		//float dvx = vx - velX;
		//velX += dvx;

		//x += velX*info.timeStep;
		//
		//if (IsKeyDown(GLFW_MOUSE_BUTTON_LEFT)) {
		//	wantX = GetMouseX();
		//	velX = 0;
		//}
	}
	void GameApp::render(engone::RenderInfo& info) {
		using namespace engone;

		Shader* shad = particleGroup->getShader();
		shad->bind();
		
		float radius = 5;
		FocalPoint focalPoint(ForceTypeAttractive, player->getPosition(),1,5,999);
		FocalPoint focalPoint2(ForceTypeRepllent,player->getPosition(),1,0,5);

		focalPoint.bind(shad,0);
		focalPoint.bind(shad,1);
		//shad->setInt("uClipping", IsKeyDown(GLFW_KEY_J));

		//part->setFloat("delta", 0);
		//part->setVec3("playerPos", {0,0,0});
		//buffer.drawPoints(1000,0);

		//group.render(info);
		//test::TestParticles(info);

		//ui::Box box = {x-25,100,50,50};
		//ui::Draw(box);

		if (player) {
			if (player->rigidBody) {
				glm::vec3 pos = player->getPosition();
				Camera* cam = m_window->getRenderer()->getCamera();
				float camH = 1.4;
				glm::mat4 camMat = glm::translate(pos + glm::vec3(0, camH, 0));
				if (player->zoom != 0) {
					camMat *= glm::rotate(cam->getRotation().y, glm::vec3(0, 1, 0)) * glm::rotate(cam->getRotation().x, glm::vec3(1, 0, 0)) * glm::translate(glm::vec3(0, 0, player->zoom));
				}
				cam->setPosition(camMat[3]);
			}
		}
		//ui::Box box = { 20,30,100,100,{1,1,1,1} };
		//ui::Draw(box);

		//glm::mat4 m1 = glm::translate(glm::vec3(1, 8, 0));
		//glm::vec3 s1(1, 1, 1);

		//glm::mat4 m2 = glm::translate(glm::vec3(1, 8, 0));
		//glm::vec3 s2(1.5, 1, 1);

		//PlaneCollider col1 = MakeCubeCollider(m1, s1);
		//PlaneCollider col2 = MakeCubeCollider(m2, s2);

		//bool boolean = TestPlanes(col1, col2);
		//
		//ModelAsset* model = GetAsset<ModelAsset>("Snake/Snake");
		//Shader* shader = GetAsset<Shader>("armature");
		//shader->Bind();

		//RenderEngine(lag);

		//AnimationAsset* anim = GetAsset<AnimationAsset>("PlayerAttack/SwingAnim");
		//frame += anim->defaultSpeed * 1.f / 60;

		//if (frame > 60) {
		//	frame = 0;
		//}

		//short usedChannels = 0;
		//glm::vec3 pos = { 0,0,0 }, euler, scale;
		//glm::mat4 quater = glm::mat4(1);
		//anim->objects[1].getValues(frame, 1,
		//	pos, euler, scale, quater, &usedChannels);

		//glm::mat4 modelMatrix = glm::translate(pos) * quater;

		//Shader* objectShader = GetAsset<Shader>("object");
		//objectShader->bind();
		//VAO.draw(&IBO,3);

		//MeshAsset* meshAsset = GetAsset<MeshAsset>("PlayerAttack/Cube.002-B");
		//MaterialAsset* material = GetAsset<MaterialAsset>("defaultMaterial");
		//MaterialAsset* material2 = GetAsset<MaterialAsset>("Material.004");

		//material2->bind(objectShader, 0);
		//material->bind(objectShader, 0);

		////log::out << quater << "\n";

		//objectShader->setMat4("uTransform", modelMatrix);

		//meshAsset->vertexArray.draw(&meshAsset->indexBuffer);

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

		//UiTest();

		//DrawCube(glm::mat4(),)

		//introScreen();

	//	Shader* objectShader = GetAsset<Shader>("object");
	//	objectShader->bind();

	//	MeshAsset* meshAsset = GetAsset<MeshAsset>("PlayerAttack/Cube.002-B");
	//	MaterialAsset* material = GetAsset<MaterialAsset>("defaultMaterial");
	//	MaterialAsset* material2 = GetAsset<MaterialAsset>("Material.004");

	//	material2->bind(objectShader, 0);
	//	material->bind(objectShader, 0);

	///*	glm::quat quat = {1.f,rot,0,0.f};
	//	rot += 1/60.f/10;*/


	//	glm::mat4 matr = glm::translate(glm::vec3(0, 5, 0)) * glm::mat4_cast(quat);
	//	objectShader->setMat4("uTransform", matr);

		//float fi[]{
		//	move / 4,-move/2, 0,
		//	-move / 4,-move, 0,
		//};
		//as->instanceBuffer.setData(6, fi);
		//as->vertexArray->selectBuffer(1, as->instanceBuffer);
		//as->vertexArray.draw(&as->indexBuffer, 2);
		//float fa[]{
		//	-move/2+0.f,0,
		//	-move/6 + 0.15f,move,
		//	move*2 + 0.1f,0.25
		//};
		//VBO2.setData(6, fa);
		//VAO.selectBuffer(1, &VBO2);
		//VAO.draw(&IBO,3);

	}
	void GameApp::onClose(engone::Window* window) {
		stop();
	}

	// ##########################
	//  Test scenes, ui and more
	// ##########################
	void GameApp::TestScene() {
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
	void GameApp::IntroScreen() {
		using namespace engone;
		//float time = GetAppTime();

		const float black = 2, fade = black + .5, intro = fade + 2, introOut = intro + .5, gameIn = introOut + .5;
		
		//if (time < black) { // black 2s
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 0.f,0.f,0.f });
		//} else if (time < fade) { // fade 0.5s
		//	float alpha = 1 - (time - black) / .5f;
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 1.f, 1.f, 1.f });
		//	ui::Draw({ GetAsset<Texture>("textures/intro"), 0.f, 0.f, GetWidth(), GetHeight() });
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 0.f,0.f,0.f, alpha });
		//} else if (time < intro) { // intro 2s
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 1.f, 1.f, 1.f });
		//	ui::Draw({ GetAsset<Texture>("textures/intro"), 0.f, 0.f, GetWidth(), GetHeight() });
		//} else if (time < introOut) { // fade 0.5s
		//	float alpha = (time - intro) / .5f;
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 1.f, 1.f, 1.f });
		//	ui::Draw({ GetAsset<Texture>("textures/intro"), 0.f, 0.f, GetWidth(), GetHeight() });
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 0.f,0.f,0.f, alpha });
		//} else if (time < gameIn) { // fade 0.5s
		//	float alpha = 1 - (time - introOut) / .5f;
		//	ui::Draw({ 0.f, 0.f, GetWidth(), GetHeight(), 0.f,0.f,0.f, alpha });
		//}

		/*IElement* cover = new IElement("darkCover", 9999);
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
					GetWindow()->lockCursor(true);
					});
				});
			});

		AddElement(blank);
		AddElement(cover);*/
	}
	//engone::ui::TextBox editText = { "012345",50,100,50 };
	//std::string hey = "Okay then";
	//int indexing = hey.length();
	void GameApp::UiTest() {
		using namespace engone;
		//editText.y += 0.05;

		/*ui::Edit(hey, indexing);
		ui::TextBox wow = { hey, 50, 100, 30 };
		wow.font = GetAsset<Font>("consolas");
		wow.at = indexing;
		wow.edited = true;
		ui::Draw(wow);*/

		/*if (editText.font == nullptr) {
			editText.font = GetAsset<Font>("consolas");
		}*/
		//ui::Draw({ "Hejsan", 100, 100, 30, 0.5f, 0.5f, 0.5f});
		/*ui::Draw({50,100,100,100,1,0,0});
		ui::Draw({GetAsset<Texture>("textures/wall"), 500, 100, 400, 400});*/

		//ui::Box fullscreen = {100+2,100,100,100,1.f,1.f,0};
		//ui::Box windowed = {100,400,100,100,1.f,0,0};
		//ui::Box borderless = {100,600,100,100,1.f,0,1.f};

		//float w = editText.font->getWidth(editText.text, editText.h);
		//ui::Draw({editText.x,editText.y,w,editText.h,1.f,0,0});

		//ui::Draw(fullscreen);
		///*ui::Draw(windowed);
		//ui::Draw(borderless);*/
		//if (ui::Hover(editText)) {
		//	ui::Edit(&editText);
		//}

		//ui::Draw(editText);

		/*if (ui::Clicked(fullscreen)) {
			GetWindow()->setMode(WindowMode::Fullscreen);
		}
		if (ui::Clicked(windowed)) {
			GetWindow()->setMode(WindowMode::Windowed);
		}
		if (ui::Clicked(borderless)) {
			GetWindow()->setMode(WindowMode::BorderlessFullscreen);
		}*/
	}

	// ##############
	//     EVENTS    
	// ##############
	engone::EventType OnKey(engone::Event& e) {
		//if (engone::CheckState(GameState::RenderGame)) {
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
				//if (engone::CheckState(GameState::Paused)) {
				//	engone::SetState(GameState::Paused, false);
				//	engone::GetActiveWindow()->lockCursor(true);
				//} else {
				//	engone::SetState(GameState::Paused, true);
				//	engone::GetActiveWindow()->lockCursor(false);
				//}
			}
		}
		//}
		return engone::EventNone;
	}
	engone::EventType OnMouse(engone::Event& e) {
		//bug::out < elementName < (elementName=="uiFade")<bug::end;
		/*if (IsGameState(Play)) {
			if (action == 0) {
				if (engine::GetPauseMode()) { // Unpause in pause menu if clicking on anything
					engine::SetPauseMode(false);
				}
			}
		}*/
		return engone::EventNone;
	}
}