
#include "ProUnk/GameApp.h"
#include "Engone/EventModule.h"

#include "ProUnk/Keybindings.h"

#include "ProUnk/Magic/Focals.h"

#include "Engone/Tests/BasicRendering.h"

#include "ProUnk/Shaders/Shaders.h"

#include "ProUnk/Magic/Magic.h"

#include "ProUnk/UI/UIMenus.h"

namespace prounk {

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

	// arguments passed to function cannot be nullptr
	void GameApp::dealCombat(engone::EngineObject* atkObj, engone::EngineObject* defObj) {
		using namespace engone;

		int atkId = atkObj->getObjectInfo();
		int defId = defObj->getObjectInfo();
		if (!atkId || !defId) {
			log::out << log::RED << "GameApp::dealCombat - one of the object's id are 0\n";
			return;
		}
		// Todo: will crash if no dimension. not good but i want to get this stuff done
		//Dimension* dim = m_session->getDimensions()[0];

		CombatData* atkData = GetCombatData(m_session, atkObj);
		CombatData* defData = GetCombatData(m_session, defObj);

		if (!atkData || !defData) {
			log::out << log::RED << "GameApp::dealCombat - atkData or defData is nullptr\n";
			return;
		}

		if (!atkData->attacking) return;

		// skip if object has been hit earlier in the attack
		bool hit = false;
		for (auto& uuid : atkData->hitObjects) {
			if (uuid == defObj->getUUID())
				hit = true;
		}
		if (hit)
			return;

		atkData->hitObjects.push_back(defObj->getUUID());

		float atk=0;
		if (atkData->damageType == CombatData::CONTINOUS_DAMAGE) {
			atk = atkData->damagePerSecond*getEngine()->getLoopInfo().timeStep;
		} else if (atkData->damageType == CombatData::SINGLE_DAMAGE) {
			atk = atkData->singleDamage;
		}
		

		defData->health -= atk;
		if (defData->health < 0) defData->health = 0;

		//getWorld()->netDamageObject(defObj->getUUID(), atk);

		// send updated health.
		// what if someone else also updates health? then do addition or subtraction on health.
		// how about hit cooldown? can other players deal damage two you at the same time?
		// i'm thinking yes.
		// still need cooldown because the same player should not be able to deal damage from the same attack twice.
		// instead of cooldown have one frame were attack deals damage? bad if frame is skipped, it won't be skipped because of how
		// the game loop works. delta is fixed. it may skip one or two frames though, depending on speed and stuff.
		// You could ensure this doesn't happen? the attack chould store which objects you hit. and then
		// don't deal damage to them. you can deal damage between an interval of frames.
		// CombatData requires vector.
		// 
		// let's not focus to much on cheating
		// 

		//glm::vec3 particlePosition = ToGlmVec3(pair.getCollider1()->getLocalToWorldTransform().getPosition() +
		//	pair.getCollider2()->getLocalToWorldTransform().getPosition()) / 2.f;
		//glm::vec3 particlePosition = defObj->getPosition();

		// spawn particles
		// hit cooldown is determined by the attacker's animation time left
		doParticles(defObj->getPosition());
		//CombatParticle* parts = combatParticles->getParticles();
		//for (int i = 0; i < combatParticles->getCount(); i++) {
		//	//float rad = glm::pi<float>() * (pow(2, 3) - pow(2 * GetRandom(), 3)) / 3.f;
		//	float rad = GetRandom() * 0.5;
		//	glm::vec3 norm = glm::vec3(GetRandom() - 0.5f, GetRandom() - 0.5f, GetRandom() - 0.5f);
		//	norm = glm::normalize(norm);
		//	parts[i].pos = particlePosition + norm * rad;
		//	parts[i].vel = norm * (0.1f + (float)GetRandom()) * 1.f;
		//	parts[i].lifeTime = 1 + GetRandom() * 3.f;
		//}

		// NOTE: if the attack anim. is restarted when the attack collider still is in the defense collider the cooldown would still be active
		//	Meaning no damage.

		float timeStep = getEngine()->getLoopInfo().timeStep;
		float knockStrength = atkData->knockStrength;
		glm::vec3 diff = (defData->owner->getPosition() - atkData->owner->getPosition());
		glm::vec3 dir = glm::normalize(diff);
		rp3d::Vector3 velocity = ToRp3dVec3(knockStrength *dir/timeStep);
		velocity += atkData->owner->getRigidBody()->getLinearVelocity() - defData->owner->getRigidBody()->getLinearVelocity();

		//rp3d::Vector3 rot = ToEuler(atkData->owner->rigidBody->getTransform().getOrientation());
		//rp3d::Vector3 force = { glm::sin(rot.y),0,glm::cos(rot.y) };
		//log::out << force << "\n";
		//rp3d::Vector3 force = def.body->getTransform().getPosition() - atk.body->getTransform().getPosition();
		//force.y = 1;
		rp3d::Vector3 force = velocity/timeStep;
		//log::out << force << "\n";
		 //for now, defense can be assumed to be a rigidbody

		defObj->getRigidBody()->applyWorldForceAtCenterOfMass(force);
	}
	void GameApp::onContact(const rp3d::CollisionCallback::CallbackData& callbackData) {
		using namespace engone;
		for (int pairI = 0; pairI < callbackData.getNbContactPairs(); pairI++) {
			auto pair = callbackData.getContactPair(pairI);

			EngineObject* obj1 = (EngineObject*)pair.getBody1()->getUserData();
			EngineObject* obj2 = (EngineObject*)pair.getBody2()->getUserData();

			//log::out << getGround()->getClient().isRunning() << " " << getGround()->getClient().isRunning() <<" "<< pair.getBody1() << " " << pair.getBody2() << "\n";

			if (!obj1 || !obj2) continue;

			if (HasCombatData(obj1->getObjectType()) && HasCombatData(obj2->getObjectType())) {
				//printf("%d - %d\n", (uint32_t)pair.getCollider1()->getUserData(), (uint32_t)pair.getCollider2()->getUserData());
				if (((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_DAMAGE) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_HEALTH))
					dealCombat(obj1, obj2);
				if (((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_HEALTH) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_DAMAGE))
					dealCombat(obj2, obj1);
			}
		}
	}
	void GameApp::onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) {
		using namespace engone;
		//log::out << "TRIGGAR\n";
		for (int pairI = 0; pairI < callbackData.getNbOverlappingPairs(); pairI++) {
			auto pair = callbackData.getOverlappingPair(pairI);
			//pair.getEventType()

			//auto pair = pair.getEventType();
			//rp3d::OverlapCallback::OverlapPair::EventType::

			EngineObject* obj1 = (EngineObject*)pair.getBody1()->getUserData();
			EngineObject* obj2 = (EngineObject*)pair.getBody2()->getUserData();

			//log::out << getGround()->getClient().isRunning() << " " << getGround()->getClient().isRunning() <<" "<< pair.getBody1() << " " << pair.getBody2() << "\n";

			if (!obj1 || !obj2) continue;

			if (HasCombatData(obj1->getObjectType()) && HasCombatData(obj2->getObjectType())) {
				//printf("%d - %d\n", (uint32_t)pair.getCollider1()->getUserData(), (uint32_t)pair.getCollider2()->getUserData());
				if(((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_DAMAGE) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_HEALTH))
					dealCombat(obj1, obj2);
				if (((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_HEALTH) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_DAMAGE))
					dealCombat(obj2, obj1);
			}
		}
	}
	GameApp::GameApp(engone::Engone* engone, GameAppInfo info) : Application(engone) {
		using namespace engone;
		
		m_window = createWindow({ ModeWindowed,1000,800 });

		m_window->setTitle("Project Unknown");
		m_window->enableFirstPerson(true);

		showSessionMenu();

		m_window->getPipeline()->addComponent(&visualEffects);

		AssetStorage* assets = getStorage();

		ShaderAsset* blur = assets->set("blur", ALLOC_NEW(ShaderAsset)(blurGLSL));
		
		//Shader* shaderPart = new Shader(particleGLSL);
		//particleGroup = new ParticleGroup<DefaultParticle>();
		//particleGroup->init(m_window, shaderPart);
		//particleGroup->createCube({ 5,0,0 }, {5,5,5 }, 1000000, {0,0,0});
		//engone->addParticleGroup(particleGroup);

		m_session = ALLOC_NEW(Session)(this);
		Dimension* firstDim = m_session->createDimension("0");

		playerController.app = this;

		//-- setup some particles
		Shader* combatPart = ALLOC_NEW(Shader)(combatParticlesGLSL);
		combatParticles = ALLOC_NEW(ParticleGroup<CombatParticle>)();
		combatParticles->init(m_window, combatPart);
		CombatParticle* parts = combatParticles->createParticles(10000);
		//for (int i = 0; i < combatParticles->getCount();i++) {
		//	float rad = 4*glm::pi<float>() * (pow(2,3) - pow(2 * GetRandom(), 3)) / 3.f;
		//	glm::vec3 norm = glm::vec3(GetRandom()-0.5f,GetRandom()-0.5f,GetRandom()-0.5f);
		//	norm = glm::normalize(norm);
		//	parts[i].pos = norm *rad;
		//	parts[i].vel = -norm*(0.33f + (float)GetRandom())*7.f;
		//	parts[i].lifeTime = 30+GetRandom()*10.f;
		//}
		
		firstDim->getWorld()->addParticleGroup(combatParticles);

		//-- Assets
		//assets->set("depth")
		//AddAsset<Shader>("depth", new Shader(depthGLSL));
		//AddAsset<Shader>("experiment", new Shader(experimentGLSL));
		//AddAsset<Shader>("test", new Shader(testGLSL));
		assets->load<FontAsset>("fonts/consolas42");

		//-- Event and game loop functions

		// ISSUE: if default keybindings change but keybindings.dat exists then the new keybinds won't apply.
		//		Temp. fix by always creating default bindings.
		//if (LoadKeybindings("data/keybindings.dat") < KEY_COUNT) {
			CreateDefualtKeybindings();
		//}

		firstDim->getWorld()->getPhysicsWorld()->setIsDebugRenderingEnabled(true);
		rp3d::DebugRenderer& debugRenderer = firstDim->getWorld()->getPhysicsWorld()->getDebugRenderer();
		debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_POINT, true);

		firstDim->getWorld()->getPhysicsWorld()->setEventListener(this);
	
		EngineObject* player = CreatePlayer(firstDim);
		playerController.setPlayerObject(player);
		player->setPosition({ 0,0,0 });
		player->setFlags(player->getFlags() | Session::OBJECT_NETMOVE);
		firstDim->getWorld()->releaseAccess(player);

		auto& playerInfo = m_session->objectInfoRegistry.getCreatureInfo(player->getObjectInfo());
		playerInfo.inventoryId = m_session->inventoryRegistry.createInventory();

		Inventory* inv = m_session->inventoryRegistry.getInventory(playerInfo.inventoryId);
		inv->resizeSlots(16, nullptr);

		registerItems();

		//ModelAsset* playerAsset = player->getModel();
		//ModelId playerModelId = m_session->modelRegistry.registerModel(playerAsset);

		ModelAsset* swordModel = assets->load<engone::ModelAsset>("SwordBase/SwordBase");
		ModelId swordModelId = m_session->modelRegistry.registerModel(swordModel);

		ItemType swordType = m_session->itemTypeRegistry.registerType("sword", swordModelId);
		Item swordItem(swordType, 1);

		ComplexData& data = *m_session->complexDataRegistry.registerData();
		ComplexPropertyType* atkProperty = m_session->complexDataRegistry.registerProperty("atk");
		data.set(atkProperty, 30.f);
		ComplexPropertyType* knockProperty = m_session->complexDataRegistry.registerProperty("knock");
		data.set(knockProperty, 0.3f);

		createPanels();

		swordItem.setComplexData(data.getDataIndex());

		inv->transferItem(swordItem);
		//EngineObject* sword = CreateItem(firstDim,swordItem);
		//sword->setFlags(sword->getFlags() | Session::OBJECT_NETMOVE);
		//sword->setPosition({ 2, 0, 0 });

		//sword->rigidBody->enableGravity(false);

		//ModelAsset* platformModel = assets->load<engone::ModelAsset>("Platform/Platform");
		//ModelAsset* daggerModel = assets->load<engone::ModelAsset>("Dagger/Dagger");

		//ModelId platId = m_session->modelRegistry.registerModel(platformModel);
		//ModelId dagId = m_session->modelRegistry.registerModel(daggerModel);

		//ItemType playerType = m_session->itemTypeRegistry.registerType("Player", playerModelId);
		//ItemType platformType = m_session->itemTypeRegistry.registerType("Platform", platId);
		//ItemType swordType = m_session->itemTypeRegistry.registerType("Sword", swordModelId);

		//inv->addItem(Item(playerType, 1));
		//inv->addItem(Item(platformType, 1));
		//inv->addItem(Item(swordType, 1));

		EngineObject* terrain = CreateTerrain(firstDim);
		terrain->setFlags(terrain->getFlags() | Session::OBJECT_NETMOVE);
		//terrain->setTransform({ 0,10,0 });
		firstDim->getWorld()->releaseAccess(terrain);
		//EngineObject* dummy = CreateDummy(firstDim);
		//dummy->setPosition({ 4,7,8 });
		//dummy->setFlags(dummy->getFlags()|Session::OBJECT_NETMOVE);
		//firstDim->getWorld()->releaseAccess(dummy);
		if (info.flags & START_SERVER) {
			SetDefaultPortIP(this, info.port, info.ip, "Server");
		} else if (info.flags & START_CLIENT) {
			SetDefaultPortIP(this, info.port, info.ip, "Client");
		}
		if (info.flags & START_SERVER) {
			
			terrain->setPosition({ 0,-2,0 });

			player->setPosition({ 1,0,0 });
			//world->getServer().start(info.port);

		} else if (info.flags & START_CLIENT) {
			//world->getClient().start(info.ip,info.port);
		}

		//rp3d::Vector3 anchor(1, 1, 1);
		//rp3d::FixedJointInfo fixedInfo(player->rigidBody,sword->rigidBody,anchor);
		//rp3d::Joint* joint = engone->m_pWorld->createJoint(fixedInfo);

		//m_window->getRenderer()->getCamera()->setPosition(1,1,2);
		//m_window->getRenderer()->getCamera()->setRotation(0,-0.5f,0);

		DirLight* dir = ALLOC_NEW(DirLight)({ -0.2,-1,-0.2 });
		engone->addLight(dir);

		delayed.start(1);
	}
	void GameApp::registerItems() {

	}
	void GameApp::panelInput(engone::LoopInfo& info) {
		using namespace engone;
		if (IsKeyPressed(GLFW_KEY_TAB)) {
			bool toggle = !inventoryPanel->getHidden();
			inventoryPanel->setHidden(toggle);
			cheatPanel->setHidden(toggle);
			if (!toggle) {
				incrCursor();
			} else {
				decrCursor();
			}
		}
		if (IsKeyPressed(GLFW_KEY_ESCAPE)) {
			bool toggle = !sessionMenu;
			sessionMenu = toggle;

			if (toggle) {
				incrCursor();
			} else {
				decrCursor();
			}
		}

		if (showCursor > 0) {
			if (engone::GetActiveWindow()->isCursorLocked()) {
				engone::GetActiveWindow()->lockCursor(false);
			}
		} else {
			if (!engone::GetActiveWindow()->isCursorLocked()) {
				engone::GetActiveWindow()->lockCursor(true);
			}
		}
	}
	void GameApp::createPanels() {
		using namespace engone;
		auto& playerInfo = m_session->objectInfoRegistry.getCreatureInfo(playerController.m_player->getObjectInfo());

		inventoryPanel = ALLOC_NEW(InventoryPanel)(this);
		panelHandler.addPanel(inventoryPanel);
		inventoryPanel->setSize(250, 230);
		inventoryPanel->setPosition(GetWidth()-260, 10);
		inventoryPanel->setInventory(playerInfo.inventoryId);

		masterInventoryPanel = ALLOC_NEW(MasterInventoryPanel)(this);
		masterInventoryPanel->setDepth(9999);
		panelHandler.addPanel(masterInventoryPanel);
		// master has no position or size.

		//craftingPanel = new CraftingPanel(this);
		//panelHandler.addPanel(craftingPanel);
		//craftingPanel->setSize(100, 100);
		//craftingPanel->setPosition(300, 400);

		playerBarPanel = ALLOC_NEW(PlayerBarPanel)(this);
		panelHandler.addPanel(playerBarPanel);
		playerBarPanel->setSize(100, 70);
		playerBarPanel->setPosition(130, 50);

		cheatPanel = ALLOC_NEW(CheatPanel)(this);
		panelHandler.addPanel(cheatPanel);
		cheatPanel->setSize(220, 100);
		cheatPanel->setPosition(GetWidth()-230, 260);

		inventoryPanel->setHidden(true);
		cheatPanel->setHidden(true);
	}
	//float wantX=500;
	//float x = 100;
	//float velX = 0;
	bool once = false;

	double travel = 0;
	int stage = 0;
	double startTime=0;
	void GameApp::update(engone::LoopInfo& info) {
		using namespace engone;

		visualEffects.update(info);

		panelInput(info);
		//if (startTime == 0) {
		//	startTime = GetSystemTime();
		//}
		//travel += 10 * info.timeStep;
		//if(stage==0) {
		//	if (travel >= 100) {
		//		double now = GetSystemTime();
		//		double diff = (now - startTime);
		//		log::out << "Done: " << diff<<" seconds ts: "<<info.timeStep<<" Travel: " << travel << " stage: " << stage << " Avg:"<< (travel/diff) << "\n";
		//		startTime = now;
		//		travel = 0;
		//		stage++;
		//		info.app->getEngine()->getStats().setUPSLimit(5);
		//	}
		//} else if(stage==1) {
		//	if (travel >= 100) {
		//		double now = GetSystemTime();
		//		double diff = (now - startTime);
		//		log::out << "Done: " << diff << " seconds ts: " << info.timeStep << " Travel: "<<travel<<" stage: "<<stage<<" Avg:" << (travel / diff) << "\n";
		//		startTime = now;
		//		stage = 0;
		//		info.app->getEngine()->getStats().setUPSLimit(60);
		//		travel = 0;
		//	}
		//}
		//log::out << "travel: "<<travel << " stage: " << stage << " \n";
		//if (engone::IsKeybindingPressed(KeyPause)) {
		//	if(engone::GetActiveWindow()->isCursorLocked()){
		//		engone::GetActiveWindow()->lockCursor(false);
		//	} else {
		//		engone::GetActiveWindow()->lockCursor(true);
		//	}
		//}
		if (partRequested) {
			CombatParticle* parts = combatParticles->getParticles();
			for (int i = 0; i < combatParticles->getCount(); i++) {
				//float rad = glm::pi<float>() * (pow(2, 3) - pow(2 * GetRandom(), 3)) / 3.f;
				float rad = GetRandom() * 0.5;
				glm::vec3 norm = glm::vec3(GetRandom() - 0.5f, GetRandom() - 0.5f, GetRandom() - 0.5f);
				norm = glm::normalize(norm);
				parts[i].pos = requestPos + norm * rad;
				parts[i].vel = norm * (0.1f + (float)GetRandom()) * 1.f;
				parts[i].lifeTime = 1 + GetRandom() * 3.f;
			}
			partRequested = false;
		}

		playerController.update(info);

		m_session->update(info);

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
	
	void GameApp::render(engone::LoopInfo& info) {
		using namespace engone;

		//visualEffects.render(info);

		//ParticleMagicTest(info,this);

		//ui::Box box = {x-25,100,50,50};
		//ui::Draw(box);

		if (sessionMenu) {
			RenderServerClientMenu(info);
		}
		// send and receive stats.
		//DebugInfo(info,this);
		
		panelHandler.render(info);

		playerController.render(info);
	}
	void GameApp::onClose(engone::Window* window) {
		stop();
	}
	Session* GameApp::getActiveSession() {
		return m_session;
	}
}