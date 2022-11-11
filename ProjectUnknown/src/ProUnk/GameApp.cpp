
#include "ProUnk/GameApp.h"
#include "Engone/EventModule.h"

#include "ProUnk/Keybindings.h"
#include "ProUnk/Objects/Player.h"
//#include "ProUnk/Objects/Sword.h"
//#include "ProUnk/Objects/Dummy.h"

#include "ProUnk/Magic/Focals.h"

#include "Engone/Tests/BasicRendering.h"

#include "ProUnk/Shaders/Shaders.h"

#include "ProUnk/Magic/Magic.h"

#include "ProUnk/UI/PlayerInformation.h"
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
	void GameApp::dealCombat(engone::GameObject* atkObj, engone::GameObject* defObj) {
		using namespace engone;

		CombatData* atkData=(CombatData*)atkObj->userData;
		CombatData* defData=(CombatData*)defObj->userData;

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

		float atk = atkData->getAttack();
		defData->health -= atk;
		if (defData->health < 0) defData->health = 0;

		getGround()->netDamageObject(defObj->getUUID(), atk);

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

		glm::vec3 diff = (defData->owner->getPosition() - atkData->owner->getPosition()) / 2.f;
		glm::vec3 dir = glm::normalize(diff);
		rp3d::Vector3 force = ToRp3dVec3(diff)*12.f;
		force += atkData->owner->rigidBody->getLinearVelocity() - defData->owner->rigidBody->getLinearVelocity();

		//rp3d::Vector3 rot = ToEuler(atkData->owner->rigidBody->getTransform().getOrientation());
		//rp3d::Vector3 force = { glm::sin(rot.y),0,glm::cos(rot.y) };
		//log::out << force << "\n";
		//rp3d::Vector3 force = def.body->getTransform().getPosition() - atk.body->getTransform().getPosition();
		//force.y = 1;
		force *= 60.f;
		//log::out << force << "\n";
		 //for now, defense can be assumed to be a rigidbody

		defObj->rigidBody->applyWorldForceAtCenterOfMass(force);
	}
	void GameApp::onContact(const rp3d::CollisionCallback::CallbackData& callbackData) {
		using namespace engone;
		for (int pairI = 0; pairI < callbackData.getNbContactPairs(); pairI++) {
			auto pair = callbackData.getContactPair(pairI);

			GameObject* ptr1 = (GameObject*)pair.getBody1()->getUserData();
			GameObject* ptr2 = (GameObject*)pair.getBody2()->getUserData();

			//log::out << getGround()->getClient().isRunning() << " " << getGround()->getClient().isRunning() <<" "<< pair.getBody1() << " " << pair.getBody2() << "\n";

			if (!ptr1 || !ptr2) continue;

			if ((ptr1->flags & OBJECT_HAS_COMBATDATA) && (ptr2->flags & OBJECT_HAS_COMBATDATA)) {
				//printf("%d - %d\n", (uint32_t)pair.getCollider1()->getUserData(), (uint32_t)pair.getCollider2()->getUserData());
				if (((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_DAMAGE) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_HEALTH))
					dealCombat(ptr1, ptr2);
				if (((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_HEALTH) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_DAMAGE))
					dealCombat(ptr2, ptr1);
			}
		}
	}
	void GameApp::onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) {
		using namespace engone;
		for (int pairI = 0; pairI < callbackData.getNbOverlappingPairs(); pairI++) {
			auto pair = callbackData.getOverlappingPair(pairI);
			//pair.getEventType()

			//auto pair = pair.getEventType();
			//rp3d::OverlapCallback::OverlapPair::EventType::

			GameObject* ptr1 = (GameObject*)pair.getBody1()->getUserData();
			GameObject* ptr2 = (GameObject*)pair.getBody2()->getUserData();

			//log::out << getGround()->getClient().isRunning() << " " << getGround()->getClient().isRunning() <<" "<< pair.getBody1() << " " << pair.getBody2() << "\n";

			if (!ptr1 || !ptr2) continue;

			if ((ptr1->flags & OBJECT_HAS_COMBATDATA )&&( ptr2->flags & OBJECT_HAS_COMBATDATA)) {
				//printf("%d - %d\n", (uint32_t)pair.getCollider1()->getUserData(), (uint32_t)pair.getCollider2()->getUserData());
				if(((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_DAMAGE) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_HEALTH))
					dealCombat(ptr1, ptr2);
				if (((uint32_t)pair.getCollider1()->getUserData() & COLLIDER_IS_HEALTH) && ((uint32_t)pair.getCollider2()->getUserData() & COLLIDER_IS_DAMAGE))
					dealCombat(ptr2, ptr1);
			}
		}
	}
	engone::EventType OnKey(engone::Event& e);
	engone::EventType OnMouse(engone::Event& e);
	GameApp::GameApp(engone::Engone* engone, GameAppInfo info) : Application(engone) {
		using namespace engone;
		
		m_window = createWindow({ ModeWindowed,1000,800 });

		m_window->setTitle("Project Unknown");
		m_window->enableFirstPerson(true);

		AssetStorage* assets = getStorage();

		ShaderAsset* blur = assets->set("blur", new ShaderAsset(blurGLSL));
		
		//Shader* shaderPart = new Shader(particleGLSL);
		//particleGroup = new ParticleGroup<DefaultParticle>();
		//particleGroup->init(m_window, shaderPart);
		//particleGroup->createCube({ 5,0,0 }, {5,5,5 }, 1000000, {0,0,0});
		//engone->addParticleGroup(particleGroup);
		NetGameGround* ground = new NetGameGround(this);
		setGround(ground);

		//-- setup some particles
		Shader* combatPart = new Shader(combatParticlesGLSL);
		combatParticles = new ParticleGroup<CombatParticle>();
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
		ground->addParticleGroup(combatParticles);

		//-- Assets
		//assets->set("depth")
		//AddAsset<Shader>("depth", new Shader(depthGLSL));
		//AddAsset<Shader>("experiment", new Shader(experimentGLSL));
		//AddAsset<Shader>("test", new Shader(testGLSL));
		assets->load<FontAsset>("fonts/consolas42");

		//-- Event and game loop functions
		m_window->attachListener(EventKey, OnKey);
		m_window->attachListener(EventClick, OnMouse);

		// ISSUE: if default keybindings change but keybindings.dat exists then the new keybinds won't apply.
		//		Temp. fix by always creating default bindings.
		//if (LoadKeybindings("data/keybindings.dat") < KEY_COUNT) {
			CreateDefualtKeybindings();
		//}

		ground->m_pWorld->setIsDebugRenderingEnabled(true);
		rp3d::DebugRenderer& debugRenderer = ground->m_pWorld->getDebugRenderer();
		debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
		//debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_POINT, true);

		ground->m_pWorld->setEventListener(this);

		player = new Player(ground);
		//player = CreatePlayer(ground);
		player->flags |= NetGameGround::OBJECT_NETMOVE;
		player->setTransform({ 0,0,0 });
		ground->addObject(player);

		GameObject* sword = CreateSword(ground);
		player->inventorySword = sword;
		sword->flags |= NetGameGround::OBJECT_NETMOVE;
		sword->setTransform({2,0,0});
		ground->addObject(sword);

		if (info.flags & START_SERVER) {
			GameObject* dummy = CreateDummy(ground);
			dummy->setTransform({ 4,7,8 });
			dummy->flags |= NetGameGround::OBJECT_NETMOVE;
			//dummy->rigidBody->setLinearVelocity({ 9,0,0 });
			ground->addObject(dummy);

			GameObject* terrain = CreateTerrain(ground);
			terrain->flags |= NetGameGround::OBJECT_NETMOVE;
			ground->addObject(terrain);
			terrain->setTransform({ 0,-2,0 });

			player->setTransform({ 1,0,0 });
			ground->getServer().start(info.port);

		} else if (info.flags & START_CLIENT) {
			ground->getClient().start(info.ip,info.port);
		} else {
			SetDefaultPortIP(this, info.port, info.ip);
		}

		//rp3d::Vector3 anchor(1, 1, 1);
		//rp3d::FixedJointInfo fixedInfo(player->rigidBody,sword->rigidBody,anchor);
		//rp3d::Joint* joint = engone->m_pWorld->createJoint(fixedInfo);

		//m_window->getRenderer()->getCamera()->setPosition(1,1,2);
		//m_window->getRenderer()->getCamera()->setRotation(0,-0.5f,0);

		DirLight* dir = new DirLight({ -0.2,-1,-0.2 });
		engone->addLight(dir);

		delayed.start(1);
	}
	//float wantX=500;
	//float x = 100;
	//float velX = 0;
	bool once = false;

	float deathTime=0;
	void GameApp::update(engone::UpdateInfo& info) {
		using namespace engone;
		if (engone::IsKeybindingPressed(KeyPause)) {
			if(engone::GetActiveWindow()->isCursorLocked()){
				engone::GetActiveWindow()->lockCursor(false);
			} else {
				engone::GetActiveWindow()->lockCursor(true);
			}
		}		
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

		//ParticleMagicTest(info,this);

		//ui::Box box = {x-25,100,50,50};
		//ui::Draw(box);

		RenderPlayerInformation(info, player);
		RenderServerClientMenu(info);

		DebugInfo(info,this);

		if (player) {
			if (player->rigidBody&&!player->isDead()) {
				glm::vec3 pos = player->getPosition();
				Camera* cam = m_window->getRenderer()->getCamera();
				float camH = 2.4;
				glm::mat4 camMat = glm::translate(pos + glm::vec3(0, camH, 0));
				if (player->zoom != 0) {
					camMat *= glm::rotate(cam->getRotation().y, glm::vec3(0, 1, 0)) * glm::rotate(cam->getRotation().x, glm::vec3(1, 0, 0)) * glm::translate(glm::vec3(0, 0, player->zoom));
				}
				cam->setPosition(camMat[3]);
			}
		}
	}
	void GameApp::onClose(engone::Window* window) {
		stop();
	}
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