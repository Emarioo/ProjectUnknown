
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
	void GameApp::dealCombat(engone::EngineObject* atkObj, engone::EngineObject* defObj, glm::vec3 contactPoint) {
		using namespace engone;

		int atkId = atkObj->getObjectInfo();
		int defId = defObj->getObjectInfo();
		if (!atkId || !defId) {
			log::out << log::RED << "GameApp::dealCombat - one of the object's id are 0\n";
			return;
		}
		// Todo: will crash if no dimension. not good but i want to get this stuff done
		//Dimension* dim = m_session->getDimensions()[0];

		CombatData* atkData = GetCombatData(atkObj);
		CombatData* defData = GetCombatData(defObj);
		
		if (!atkData || !defData) {
			log::out << log::RED << "GameApp::dealCombat - atkData or defData is nullptr\n";
			return;
		}

		if (!atkData->attacking) return;

		if (atkObj->getObjectType() == OBJECT_DUMMY && defObj->getObjectType() == OBJECT_DUMMY)
			return;

		if (!(atkObj->getFlags() & Session::OBJECT_NETMOVE)) // ensure collision is done once, one of the clients or server not all of them.
			return;
		// can't attack themself
		if (atkData->owner == defObj) {
			return;
		}

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
		
		if (defData->health == 0)
			return;

		defData->health -= atk;
		if (defData->health < 0)
			defData->health = 0;
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
		//doParticles(defObj->getPosition());
		visualEffects.addDamageParticle(contactPoint);
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

		char buffer[15];
		snprintf(buffer, sizeof(buffer), "%.1f", atk);
		//glm::vec4 numColor = { GetRandom(),GetRandom(),GetRandom(),1 };
		glm::vec4 numColor = {1, 0.1, 0.05, 1};
		visualEffects.addDamageNumber(buffer, contactPoint, numColor);

		// NOTE: if the attack anim. is restarted when the attack collider still is in the defense collider the cooldown would still be active
		//	Meaning no damage.

		glm::vec3 knockDirection{0,0,0};
		
		float timeStep = getEngine()->getLoopInfo().timeStep;
		float knockStrength = atkData->knockStrength;
		glm::vec3 diff = (defData->owner->getPosition() - atkData->owner->getPosition());
		float leng = glm::length(diff);
		if (leng != 0) {
			glm::vec3 dir = diff / leng;
			glm::vec3 velocity = knockStrength * dir / timeStep;
			velocity += atkData->owner->getLinearVelocity() - defData->owner->getLinearVelocity();

			//rp3d::Vector3 rot = ToEuler(atkData->owner->rigidBody->getTransform().getOrientation());
			//rp3d::Vector3 force = { glm::sin(rot.y),0,glm::cos(rot.y) };
			//log::out << force << "\n";
			//rp3d::Vector3 force = def.body->getTransform().getPosition() - atk.body->getTransform().getPosition();
			//force.y = 1;
			knockDirection = velocity / timeStep;
			//log::out << "Apply force " << force << "\n";
			//for now, defense can be assumed to be a rigidbody
			defObj->applyForce(knockDirection);
		}

		getActiveSession()->netDamageObject(defObj, atk, knockDirection);
	}
	void GameApp::onContact(const rp3d::CollisionCallback::CallbackData& callbackData) {
		using namespace engone;
		for (int pairI = 0; pairI < callbackData.getNbContactPairs(); pairI++) {
			auto pair = callbackData.getContactPair(pairI);

			if (pair.getEventType() == rp3d::CollisionCallback::ContactPair::EventType::ContactExit)
				continue;

			EngineObject* obj1 = (EngineObject*)pair.getBody1()->getUserData();
			EngineObject* obj2 = (EngineObject*)pair.getBody2()->getUserData();
			//continue;
			//log::out << getGround()->getClient().isRunning() << " " << getGround()->getClient().isRunning() <<" "<< pair.getBody1() << " " << pair.getBody2() << "\n";

			if (!obj1 || !obj2) continue;
			
			// Checking for combat data instead of creature because a barrel could be broken but isn't a creature.
			if (HasCombatData(obj1->getObjectType()) && HasCombatData(obj2->getObjectType())) {
				//printf("%d - %d\n", (uint32_t)pair.getCollider1()->getUserData(), (uint32_t)pair.getCollider2()->getUserData());
				int count = pair.getNbContactPoints();

				glm::vec3 contactPoint;
				if (count != 0) {
					auto cp = pair.getContactPoint(0);
					rp3d::Vector3 vec = pair.getCollider1()->getLocalToWorldTransform() * cp.getLocalPointOnCollider1();
					contactPoint = ToGlmVec3(vec);
				} else {
					rp3d::Vector3 mid = (pair.getBody1()->getTransform().getPosition() + pair.getBody2()->getTransform().getPosition()) / 2;
					contactPoint = ToGlmVec3(mid);
				}

				if (((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_DAMAGE) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_HEALTH))
					dealCombat(obj1, obj2, contactPoint);
				if (((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_HEALTH) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_DAMAGE))
					dealCombat(obj2, obj1, contactPoint);
			}
		}
	}
	void GameApp::onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) {
		using namespace engone;
		//log::out << "TRIGGAR\n";
		for (int pairI = 0; pairI < callbackData.getNbOverlappingPairs(); pairI++) {
			auto pair = callbackData.getOverlappingPair(pairI);
			//pair.getEventType()
			//continue;
			//auto pair = pair.getEventType();
			//rp3d::OverlapCallback::OverlapPair::EventType::
			
			EngineObject* obj1 = (EngineObject*)pair.getBody1()->getUserData();
			EngineObject* obj2 = (EngineObject*)pair.getBody2()->getUserData();

			//log::out << getGround()->getClient().isRunning() << " " << getGround()->getClient().isRunning() <<" "<< pair.getBody1() << " " << pair.getBody2() << "\n";

			if (!obj1 || !obj2) continue;
			
			if (HasCombatData(obj1->getObjectType()) && HasCombatData(obj2->getObjectType())) {
				rp3d::Vector3 cp2 = (pair.getCollider1()->getLocalToWorldTransform().getPosition() + pair.getCollider2()->getLocalToWorldTransform().getPosition()) / 2;
				//rp3d::Vector3 cp2 = (pair.getBody1()->getTransform().getPosition() + pair.getBody2()->getTransform().getPosition()) / 2;
				//rp3d::Vector3 cp2 = (pair.getBody1()->getTransform().getPosition() + pair.getBody2()->getTransform().getPosition()) / 2;
				glm::vec3 contactPoint = ToGlmVec3(cp2);
				//printf("%d - %d\n", (uint32_t)pair.getCollider1()->getUserData(), (uint32_t)pair.getCollider2()->getUserData());
				if(((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_DAMAGE) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_HEALTH))
					dealCombat(obj1, obj2, contactPoint);
				if (((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_HEALTH) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_DAMAGE))
					dealCombat(obj2, obj1, contactPoint);
			}
		}
	}
	GameApp::GameApp(engone::Engone* engone, GameAppInfo info) : Application(engone) {
		using namespace engone;
		
		m_window = createWindow({ ModeWindowed,1000,800 });

		m_window->setTitle("Project Unknown");
		m_window->enableFirstPerson(true);

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

		// This should be moved into engone
		firstDim->getWorld()->lockPhysics();
		//firstDim->getWorld()->getPhysicsWorld()->setIsDebugRenderingEnabled(true);
		//rp3d::DebugRenderer& debugRenderer = firstDim->getWorld()->getPhysicsWorld()->getDebugRenderer();
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
		firstDim->getWorld()->getPhysicsWorld()->setEventListener(this);
		firstDim->getWorld()->unlockPhysics();

		//auto logger = getPhysicsCommon()->createDefaultLogger();
		//rp3d::uint logLevel = static_cast<rp3d::uint>(static_cast<rp3d::uint>(rp3d::Logger::Level::Warning) | static_cast<rp3d::uint>(rp3d::Logger::Level::Error)| static_cast<rp3d::uint>(rp3d::Logger::Level::Information));
		//// Output the logs into an HTML file 
		//logger->addFileDestination("rp3d_log_.html", logLevel, rp3d::DefaultLogger::Format::HTML);
		//// Output the logs into the standard output 
		//logger->addStreamDestination(std::cout, logLevel, rp3d::DefaultLogger::Format::Text);
		//// Set the logger 
		//getPhysicsCommon()->setLogger(logger);


		EngineObject* player = CreatePlayer(firstDim);
		//playerController.setPlayerObject(player);
		playerController.setPlayerObject(player->getUUID());
		player->setPosition({ 0,0,0 });
		firstDim->getWorld()->releaseAccess(player->getUUID());

		auto& playerInfo = m_session->objectInfoRegistry.getCreatureInfo(player->getObjectInfo());
		playerInfo.inventoryDataIndex = m_session->inventoryRegistry.createInventory();

		Inventory* inv = m_session->inventoryRegistry.getInventory(playerInfo.inventoryDataIndex);
		inv->resizeSlots(16, nullptr);

		registerItems();

		createPanels();

		// Temporary? one function to do this?
		sessionPanel->setHidden(false);
		incrCursor();
		
		EngineObject* dummy = CreateDummy(firstDim);
		dummy->setPosition({ 4,7,8 });
		firstDim->getWorld()->releaseAccess(dummy->getUUID());
		
		//-- Setup network stuff
		sessionPanel->setDefaultPortIP(info.port, info.ip, "Client");
		if (info.flags & START_SERVER) {
			sessionPanel->setDefaultPortIP(info.port, info.ip, "Server");
		} else if (info.flags & START_CLIENT) {
			sessionPanel->setDefaultPortIP(info.port, info.ip, "Client");
		}

		//if (info.flags != START_CLIENT) {
			EngineObject* terrain = CreateTerrain(firstDim, "ProtoArena/ProtoArena");
			firstDim->getWorld()->releaseAccess(terrain->getUUID());
		//}

		if (info.flags & START_SERVER) {
			m_session->getServer().start(info.port);
		} else if (info.flags & START_CLIENT) {
			m_session->getClient().start(info.ip,info.port);
		}

		//-- Other
		DirLight* dir = ALLOC_NEW(DirLight)({ -0.2,-1,-0.2 });
		engone->addLight(dir);

		delayed.start(1);
	}
	GameApp::~GameApp() {
		cleanup();
	}
	void GameApp::cleanup() {
		using namespace engone;
		log::out << log::RED << "GameApp : CLEANUP NOT IMPLEMENTED\n";
	}
	void GameApp::registerItems() {
		ComplexPropertyType* atkProperty = m_session->complexDataRegistry.registerProperty("atk");
		ComplexPropertyType* knockProperty = m_session->complexDataRegistry.registerProperty("knock");
		
		ModelId swordModel = m_session->modelRegistry.registerModel("Sword/Sword");
		ModelId daggerModel = m_session->modelRegistry.registerModel("Dagger/Dagger");
		ModelId spearModel = m_session->modelRegistry.registerModel("Spear/Spear");
		ModelId goldModel = m_session->modelRegistry.registerModel("GoldIngot/GoldIngot");

		ItemType sword = m_session->itemTypeRegistry.registerType("sword", swordModel);
		ItemType dagger = m_session->itemTypeRegistry.registerType("dagger", daggerModel);
		ItemType spear = m_session->itemTypeRegistry.registerType("spear", spearModel);
		
		ItemType gold_ingot = m_session->itemTypeRegistry.registerType("gold_ingot", goldModel);

		auto plr = playerController.requestPlayer();
		Item godSword = { sword,1 };
		godSword.getComplexData()->set(atkProperty, 999);
		GetInventory(plr)->transferItem(godSword);
		playerController.releasePlayer(plr);
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
			bool toggle = !sessionPanel->getHidden();
			sessionPanel->setHidden(toggle);

			if (!toggle) {
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

		if (IsKeyPressed(GLFW_KEY_O)) {
			info.app->getEngine()->setFlags(info.app->getEngine()->getFlags()^Engone::EngoneShowHitboxes);
		}
	}
	void GameApp::createPanels() {
		using namespace engone;
		EngineObject* plr = playerController.requestPlayer();

		auto& playerInfo = m_session->objectInfoRegistry.getCreatureInfo(plr->getObjectInfo());

		playerController.releasePlayer(plr);

		inventoryPanel = ALLOC_NEW(InventoryPanel)(this);
		panelHandler.addPanel(inventoryPanel);
		inventoryPanel->setSize(250, 230);
		inventoryPanel->setPosition(GetWidth()-260, 10);
		inventoryPanel->setInventory(playerInfo.inventoryDataIndex);

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

		sessionPanel = ALLOC_NEW(SessionPanel)(this);
		panelHandler.addPanel(sessionPanel);
		sessionPanel->setSize(230, 140);
		sessionPanel->setPosition(0, GetHeight() / 2);

		sessionPanel->setHidden(true);
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

		//if (partRequested) {
		//	CombatParticle* parts = combatParticles->getParticles();
		//	for (int i = 0; i < combatParticles->getCount(); i++) {
		//		//float rad = glm::pi<float>() * (pow(2, 3) - pow(2 * GetRandom(), 3)) / 3.f;
		//		float rad = GetRandom() * 0.5;
		//		glm::vec3 norm = glm::vec3(GetRandom() - 0.5f, GetRandom() - 0.5f, GetRandom() - 0.5f);
		//		norm = glm::normalize(norm);
		//		parts[i].pos = requestPos + norm * rad;
		//		parts[i].vel = norm * (0.1f + (float)GetRandom()) * 1.f;
		//		parts[i].lifeTime = 1 + GetRandom() * 3.f;
		//	}
		//	partRequested = false;
		//}

		playerController.update(info);

		m_session->update(info);
	}
	
	void GameApp::render(engone::LoopInfo& info) {
		using namespace engone;

		//ParticleMagicTest(info,this);

		//ui::Box box = {x-25,100,50,50};
		//ui::Draw(box);

		// send and receive stats.
		if (IsKeyPressed(GLFW_KEY_F2))
			debugInfoToggle = !debugInfoToggle;
		if(debugInfoToggle)
			DebugInfo(info,this);
		
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