#include "ProUnk/PlayerController.h"

#include "Engone/EventModule.h"
#include "ProUnk/Keybindings.h"
#include "Engone/Window.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/Combat/Combat.h"

#include "ProUnk/Objects/BasicObjects.h"

namespace prounk {
	PlayerController::PlayerController() {
		
	}
	void PlayerController::setPlayerObject(engone::UUID player) {
		m_playerId = player;

		setFlight(true);
	}
	//void PlayerController::setPlayerObject(engone::EngineObject* player) {
	//	m_player = player;

	//	setFlight(true);
	//}
	engone::EngineObject* PlayerController::requestPlayer() {
		Dimension* dim = app->getActiveSession()->getDimension("0");
		engone::EngineObject* plr = dim->getWorld()->requestAccess(m_playerId);
		return plr;
	}
	void PlayerController::releasePlayer(engone::EngineObject* plr) {
		if (!plr) return;
		Dimension* dim = app->getActiveSession()->getDimension("0");
		dim->getWorld()->releaseAccess(plr->getUUID());
	}
	engone::EngineObject* PlayerController::requestHeldObject() {
		Dimension* dim = app->getActiveSession()->getDimension("0");
		engone::EngineObject* held = dim->getWorld()->requestAccess(heldObjectId);
		return held;
	}
	void PlayerController::releaseHeldObject(engone::EngineObject* held) {
		if (!held) return;
		Dimension* dim = app->getActiveSession()->getDimension("0");
		dim->getWorld()->releaseAccess(held->getUUID());
	}
	void PlayerController::update(engone::LoopInfo& info) {
		//animator.update(info.timeStep);
		Input(info);
		Movement(info);
		WeaponUpdate(info);
	}
	void PlayerController::setDead(bool yes) {
		engone::EngineObject* plr = requestPlayer();
		if (plr) {
			if (yes)
				plr->setFlags(plr->getFlags() | OBJECT_IS_DEAD);
			else
				plr->setFlags(plr->getFlags() & (~OBJECT_IS_DEAD));
			releasePlayer(plr);
		}
	}
	bool PlayerController::isDead() {
		engone::EngineObject* plr = requestPlayer();
		bool yes = false;
		if (plr) {
			yes = plr->getFlags() & OBJECT_IS_DEAD;
			releasePlayer(plr);
		}
		return yes;
	}
	void PlayerController::WeaponUpdate(engone::LoopInfo& info) {
		using namespace engone;
		
		EngineObject* plr = requestPlayer();
		if (!plr)
			return;
		// first slot in inventory is the held weapon.
		Inventory* inv = getInventory();

		Dimension* dim = app->getActiveSession()->getDimensions()[0];

		EngineObject* heldObject = requestHeldObject();

		if (inv->getSlotSize() != 0) {
			auto& item = inv->getItem(0);
			//log::out << "Type: "<<item.getType() << "\n";
			if (lastItemType != item.getType()) {
				if (heldObject) {
					if (item.getType() == 0) {
						// remove held object
						app->getActiveSession()->netDeleteObject(heldObject);
						DeleteObject(dim,heldObject);
						heldObjectId = 0;
						heldObject = nullptr;
					} else {
						// reuse held object
						heldObject->setModel(dim->getParent()->modelRegistry.getModel(item.getModelId()));
						heldObject->setOnlyTrigger(true);
						app->getActiveSession()->netSetTrigger(heldObject, true);
					}
				} else {
					if (item.getType() != 0) {
						// create new object
						ModelAsset* model = dim->getParent()->modelRegistry.getModel(item.getModelId());
						heldObject = CreateWeapon(dim, model->getLoadName());
						heldObjectId = heldObject->getUUID();
						heldObject->setOnlyTrigger(true);
						
						app->getActiveSession()->netAddGeneral(heldObject);
						app->getActiveSession()->netSetTrigger(heldObject,true);
					}
					// damage in combat data is set when doing a skill
				}
				lastItemType = item.getType();
			} else {
				// do nothing if types are the same
			}
		}

		// maintain held object at right position

		if (!heldObject) {
			releasePlayer(plr);
			return;
		}

		if (plr->getFlags() & OBJECT_IS_DEAD) {
			if (heldObject->isOnlyTrigger()) {
				heldObject->setOnlyTrigger(false);
				app->getActiveSession()->netSetTrigger(heldObject, false);
			}
			releasePlayer(plr);
			releaseHeldObject(heldObject);
			return;
		}
		if (!heldObject->isOnlyTrigger()) {
			heldObject->setOnlyTrigger(true);
			//app->getActiveSession()->netSetTrigger(heldObject, true);
		}

		ModelAsset* model = plr->getModel();
		rp3d::RigidBody* body = plr->getRigidBody();

		if (!model->valid()) {
			releasePlayer(plr);
			releaseHeldObject(heldObject);
			return;
		}
		//log::out << "WOW\n";
		auto transforms = model->getParentTransforms(nullptr);

		rp3d::Transform swordTrans;

		glm::mat4 modelMatrix = ToMatrix(body->getTransform());

		AssetInstance* inst = nullptr;
		ArmatureAsset* arm = nullptr;
		glm::mat4 instMat;
		for (int i = 0; i < model->instances.size(); i++) {
			auto& instance = model->instances[i];
			if (instance.asset->type == ArmatureAsset::TYPE) {
				inst = &instance;
				arm = (ArmatureAsset*)inst->asset;
				instMat = transforms[i] * inst->localMat;
			}
		}
		if (inst != nullptr) {
			std::vector<glm::mat4> baseBoneMats;
			std::vector<glm::mat4> boneMats = model->getArmatureTransforms(plr->getAnimator(), instMat, inst, arm, &baseBoneMats);
			Bone& lastBone = arm->bones.back();

			glm::mat4 gripMat = baseBoneMats.back();
			swordTrans = ToTransform(modelMatrix * gripMat);
		}

		dim->getWorld()->lockPhysics();
		heldObject->getRigidBody()->setTransform(swordTrans);

		heldObject->getRigidBody()->setAngularVelocity(body->getAngularVelocity());
		heldObject->getRigidBody()->setLinearVelocity(body->getLinearVelocity());
		heldObject->getRigidBody()->resetForce();
		heldObject->getRigidBody()->resetTorque();
		dim->getWorld()->unlockPhysics();

		// cleanup
		releasePlayer(plr);
		releaseHeldObject(heldObject);

		//heldWeapon->rigidBody->enableGravity(rigidBody->isGravityEnabled());
		//weaponState.sample(heldWeapon);
	}
	void PlayerController::setFlight(bool yes) {
		using namespace engone;
		EngineObject* plr = requestPlayer();
		if (!plr) return;

		flight = yes;
		plr->getWorld()->lockPhysics();

		plr->getRigidBody()->enableGravity(!yes);
		plr->getRigidBody()->setLinearDamping(yes? 7.0 : 0.f);

		plr->getWorld()->unlockPhysics();
		releasePlayer(plr);
	}
	void PlayerController::setNoClip(bool yes) {
		using namespace engone;
		noclip = yes;
		EngineObject* plr = requestPlayer();
		if (!plr) return;
		plr->setOnlyTrigger(yes);
		releasePlayer(plr);
	}
	
	engone::EngineObject* PlayerController::getSeenObject() {
		using namespace engone;

		CommonRenderer* renderer = GET_COMMON_RENDERER();
		
		if (!renderer) {
			log::out << log::RED << "PlayerController::getSeenObject : renderer is null\n";
			return nullptr;
		}
		Camera* camera = renderer->getCamera();

		auto& dims = app->getActiveSession()->getDimensions();
		if (dims.size() == 0) {
			return nullptr;
		}
		Dimension* dim = dims[0]; // Todo: the dimension should be the one the player is in.

		// do raycast, pick up item if it is an item.
		glm::vec3 lookDir = camera->getLookVector();
		float distance = 8;
		
		glm::vec3 from = camera->getPosition() + camera->getLookVector() * (zoom);
		glm::vec3 to = camera->getPosition() + camera->getLookVector() * (distance + zoom);

		rp3d::Ray ray(ToRp3dVec3(from), ToRp3dVec3(to));
		
		auto objects = dim->getWorld()->raycast(ray, 1,OBJECT_PLAYER);
		
		if (objects.size() != 0)
			return objects[0];
		else
			return nullptr;
	}
	bool PlayerController::pickupItem(engone::EngineObject* object) {
		using namespace engone;
		
		// TODO: add log messages?

		if (!object)
			return false;
		if (object->getObjectType() != OBJECT_ITEM)
			return false;

		auto& dims = app->getActiveSession()->getDimensions();
		if (dims.size() == 0)
			return false;

		Dimension* dim = dims[0]; // Todo: the dimension should be the one the player is in.
		EngineWorld* world = dim->getWorld();
		
		auto& itemInfo = app->getActiveSession()->objectInfoRegistry.getItemInfo(object->getObjectInfo());
		if (!itemInfo.item.getType())
			return false;

		Inventory* inv = getInventory();

		bool transferred = inv->transferItem(itemInfo.item);
		//int slot = inv->findAvailableSlot(itemInfo.item);
		//if (slot >= 0)
		//	itemInfo.item.copy(inv->getItem(slot));

		if (!transferred)
			return false;

		// delete item object if succefully added to inventory
		app->getActiveSession()->netDeleteObject(object);
		DeleteObject(dim, object); // careful with memory leaks. functions is not complete
		return true;
	}
	Inventory* PlayerController::getInventory() {
		using namespace engone;
		EngineObject* plr = requestPlayer();
		auto& playerInfo = app->getActiveSession()->objectInfoRegistry.getCreatureInfo(plr->getObjectInfo());
		releasePlayer(plr);
		Inventory* inv = app->getActiveSession()->inventoryRegistry.getInventory(playerInfo.inventoryDataIndex);
		return inv;
	}
	void PlayerController::dropItem(int slotIndex, int count) {
		using namespace engone;
		Inventory* inv = getInventory();
		if (count <= 0) return;

		if (inv->getSlotSize() <=slotIndex)
			return;

		Item& item = inv->getItem(slotIndex);
		if (item.getType() == 0)
			return;
	
		auto& dims = app->getActiveSession()->getDimensions();
		if (dims.size() == 0)
			return;
		Dimension* dim = dims[0]; // Todo: the dimension should be the one the player is in.

		auto heldObject = requestHeldObject();
		auto object = CreateItem(dim,item);
		item = Item(); // clear slot
		if (heldObject) {
			object->getRigidBody()->setTransform(heldObject->getRigidBody()->getTransform());
		} else {
			EngineObject* plr = requestPlayer();
			object->setPosition(plr->getPosition());
			releasePlayer(plr);
		}
		app->getActiveSession()->netAddGeneral(object);
		releaseHeldObject(heldObject);
	}
	void PlayerController::dropAllItems() {
		using namespace engone;

		EngineObject* plr = requestPlayer();
		if (!plr) return;
		Inventory* inv = getInventory();

		auto& dims = app->getActiveSession()->getDimensions();
		if (dims.size() == 0) {
			releasePlayer(plr);
			return;
		}

		// Drops the held object at a specific position
		Dimension* dim = dims[0]; // Todo: the dimension should be the one the player is in.
		auto heldObject = requestHeldObject();
		for (int i = 0; i < inv->getSlotSize();i++) {
			Item& item = inv->getItem(i);

			if (item.getType() == 0)
				continue;

			auto object = CreateItem(dim, item);
			item = Item(); // clear slot

			if (i == 0) {
				// use held object's position when it drops.
				if (heldObject) {
					dim->getWorld()->lockPhysics();
					object->getRigidBody()->setTransform(heldObject->getRigidBody()->getTransform());
					dim->getWorld()->unlockPhysics();
				}
			} else {
				// use a random position for items not equipped.
				glm::vec3 offset = { GetRandom() - .5f, 1 + GetRandom() - .5f,GetRandom() - .5f };
				offset *= .5;
				object->setPosition(plr->getPosition() + offset);
			}

			dim->getWorld()->lockPhysics();
			rp3d::Vector3 force = { (float)GetRandom() - .5f, (float)GetRandom(),(float)GetRandom() - .5f };
			force *= 200.f * deathItemShockStrength;
			object->getRigidBody()->applyWorldForceAtCenterOfMass(force);

			rp3d::Vector3 torque = { (float)GetRandom() - .5f, (float)GetRandom()-.5f,(float)GetRandom() - .5f };
			torque *= 600.f * deathItemShockStrength;
			object->getRigidBody()->applyWorldForceAtCenterOfMass(torque);
			dim->getWorld()->unlockPhysics();

			app->getActiveSession()->netAddGeneral(object);

			dim->getWorld()->releaseAccess(object->getUUID());
		}
		releaseHeldObject(heldObject);
		releasePlayer(plr);
	}
	void PlayerController::performSkills(engone::LoopInfo& info) {
		using namespace engone;
		EngineObject* plr = requestPlayer();
		if (!plr) return;
		
		CombatData* weaponCombat = nullptr;
		auto heldObject = requestHeldObject();
		if (heldObject)
			weaponCombat = GetCombatData(app->getActiveSession(), heldObject);

		if (!weaponCombat) {
			releasePlayer(plr);
			releaseHeldObject(plr);
			return;
		}

		//releaseHeldObject(heldObject);
		//log::out << "frame: " <<m_player->getAnimator()->enabledAnimations[0].frame << "\n";

		if (IsKeyDown(GLFW_MOUSE_BUTTON_LEFT)) {
			if (!weaponCombat->attacking) {
				// Set damage of weapon
				weaponCombat->damageType = CombatData::SINGLE_DAMAGE;
				Inventory* inv = getInventory();
				Item& firstItem = inv->getItem(0);

				ComplexData* complexData = firstItem.getComplexData();
				//ComplexData* complexData = app->getActiveSession()->complexDataRegistry.getData(firstItem.getComplexData());
				if (complexData) { // cannot attack without data
					ComplexPropertyType* atkProperty = app->getActiveSession()->complexDataRegistry.getProperty("atk");
					ComplexPropertyType* knockProperty = app->getActiveSession()->complexDataRegistry.getProperty("knock");
					weaponCombat->singleDamage = complexData->get(atkProperty);
					weaponCombat->knockStrength = complexData->get(knockProperty);

					char buffer[15];
					snprintf(buffer,sizeof(buffer), "%.1f", weaponCombat->singleDamage);
					// other things
					glm::vec4 color = { GetRandom(),GetRandom(),GetRandom(),1 };
					//glm::vec4 color = {1, 0.1, 0.05, 1};
					app->visualEffects.addDamageNumber(buffer,heldObject->getPosition(), color);
					
					weaponCombat->skillType = SKILL_SLASH;
					weaponCombat->attack();
					plr->getAnimator()->enable("Player", "PlayerDownSwing", { false,1,1,0 });
					app->getActiveSession()->netAnimateObject(plr, "Player", "PlayerDownSwing", false, 1, 1, 0);
					AnimatorProperty* prop = plr->getAnimator()->getProp("Player");
					if (prop) {
						weaponCombat->animationTime = prop->getRemainingSeconds();
					}
				} else {
					log::out << "PlayerController::performSkills : Weapon is missing complex data\n";
				}
			}
		}
		if (IsKeyDown(GLFW_KEY_F)) {
			if (!weaponCombat->attacking) {
				// Set damage of weapon
				weaponCombat->damageType = CombatData::SINGLE_DAMAGE;
				Inventory* inv = getInventory();
				Item& firstItem = inv->getItem(0);

				ComplexData* complexData = firstItem.getComplexData();
				//ComplexData* complexData = app->getActiveSession()->complexDataRegistry.getData(firstItem.getComplexData());
				if (complexData) {
					ComplexPropertyType* atkProperty = app->getActiveSession()->complexDataRegistry.getProperty("atk");
					ComplexPropertyType* knockProperty = app->getActiveSession()->complexDataRegistry.getProperty("knock");
					weaponCombat->singleDamage = complexData->get(atkProperty);
					weaponCombat->knockStrength = complexData->get(knockProperty); // side knock?

					weaponCombat->singleDamage *= 1.1f; // amplify damage a little with this skill

					// other stuff
					weaponCombat->skillType = SKILL_SIDE_SLASH;
					weaponCombat->attack();
					plr->getAnimator()->enable("Player", "PlayerSideSwing", { false,1,1,0 });
					app->getActiveSession()->netAnimateObject(plr, "Player", "PlayerSideSwing", false, 1, 1, 0);
					AnimatorProperty* prop = plr->getAnimator()->getProp("Player");
					if (prop) {
						weaponCombat->animationTime = prop->getRemainingSeconds();
					}
				} else {
					log::out << "PlayerController::performSkills : Weapon is missing complex data\n";
				}
			}
		}
		releasePlayer(plr);
		releaseHeldObject(plr);
	}
	void PlayerController::Input(engone::LoopInfo& info) {
		using namespace engone;
		if (IsKeyPressed(GLFW_KEY_G)) {
			setFlight(!flight);
		}
		if (IsKeyPressed(GLFW_KEY_C)) {
			setNoClip(!noclip);
		}
		hoveredItem.clear();
		EngineObject* obj = getSeenObject();
		if (obj) {
			if (obj->getObjectType() == OBJECT_ITEM) {
				ObjectItemInfo& info = app->getActiveSession()->objectInfoRegistry.getItemInfo(obj->getObjectInfo());
				hoveredItem = info.item.getDisplayName();
			}
		}
		//if (IsKeyPressed(GLFW_KEY_E)) {
		if (IsKeyDown(GLFW_KEY_E)) {
			pickupItem(obj);
		}
		if (IsKeyPressed(GLFW_KEY_Q)) {
			dropItem(0,1);
		}
		//Dimension* dim = app->getActiveSession()->getDimensions()[0];

		EngineObject* plr = requestPlayer();

		CombatData* playerCombat = GetCombatData(app->getActiveSession(), plr);
		if (IsKeyPressed(GLFW_KEY_R)) {
			if (isDead()) {
				deathTime = 0;
				setDead(false);
				plr->getWorld()->lockPhysics();
				plr->getRigidBody()->setAngularLockAxisFactor({0,1,0});
				//auto tr = rigidBody->getTransform(); // only reset orientation
				//tr.setOrientation({ 0,0,0,1.f });
				//rigidBody->setTransform(tr);
				plr->getRigidBody()->setTransform({}); // reset position too
				plr->getWorld()->unlockPhysics();

				playerCombat->health = playerCombat->getMaxHealth();
				app->getActiveSession()->netSetHealth(plr, playerCombat->health);
			}
		}
		if (IsKeyPressed(GLFW_KEY_K)) {
			playerCombat->health = 0; // kill player
			app->getActiveSession()->netSetHealth(plr, playerCombat->health);
		}
		if (IsKeyPressed(GLFW_KEY_L)) {
			playerCombat->health = playerCombat->getMaxHealth();
			app->getActiveSession()->netSetHealth(plr, playerCombat->health);
		}
		performSkills(info);
		releasePlayer(plr);
	}
	void PlayerController::render(engone::LoopInfo& info) {
		using namespace engone;

		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		if (!hoveredItem.empty()) {
			ui::TextBox textBox = { hoveredItem, 0, 0, 30, consolas, {0,1.f,1.f,1.f}};
			textBox.x = GetWidth() / 2 - textBox.getWidth()/2;
			textBox.y = GetHeight() / 2 - textBox.getHeight()/2;
			ui::Draw(textBox);
		}
		EngineObject* plr = requestPlayer();
		if (plr->getRigidBody() && !isDead()) {
			glm::vec3 pos = plr->getPosition();
			CommonRenderer* renderer = GET_COMMON_RENDERER();
			if (!renderer) {
				log::out << log::RED << "PlayerController::render : renderer is null\n";
			} else {
				Camera* cam = renderer->getCamera();
				//glm::mat4 camMat = glm::translate(pos + camOffset);
				glm::vec3 camPos = pos + camOffset - cam->getLookVector() * zoom;
				//if (zoom != 0) {
				//	camMat *= glm::rotate(cam->getRotation().y, glm::vec3(0, 1, 0)) 
				//		* glm::rotate(cam->getRotation().x, glm::vec3(1, 0, 0)) 
				//		* glm::translate(glm::vec3(0, 0, zoom));
				//}
				//cam->setPosition(camMat[3]);
				cam->setPosition(camPos);
			}
		}
		else {
			//log::out << "DEAD\n";
		}
		releasePlayer(plr);
	}
	void PlayerController::Movement(engone::LoopInfo& info) {
		using namespace engone;

		Window* window = engone::GetActiveWindow();
		if (!window) return;
		CommonRenderer* renderer = GET_COMMON_RENDERER();
		//Renderer* renderer = window->getRenderer(); // if window is valid, render and camera should as well. BUT i may change somethings which would break this assumption. So i'll keep it.
		if (!renderer) return;
		Camera* camera = renderer->getCamera();
		if (!camera) return;

		float speed = walkSpeed;
		if (flight)
			speed = flySpeed;
		if (IsKeybindingDown(KeySprint)) {
			if (flight)
				speed = flyFastSpeed;
			else
				speed = sprintSpeed;
		}

		glm::vec3 flatInput = { 0,0,0 };
		if (IsKeybindingDown(KeyForward)) {
			flatInput.z += 1;
		}
		if (IsKeybindingDown(KeyLeft)) {
			flatInput.x -= 1;
		}
		if (IsKeybindingDown(KeyBack)) {
			flatInput.z -= 1;
		}
		if (IsKeybindingDown(KeyRight)) {
			flatInput.x += 1;
		}

		if (!isDead()&&engone::GetActiveWindow()->isCursorLocked()) {
			float zoomAcc = 0.1 + 5 * abs(zoomSpeed);
			if (zoomSpeed > 0) {
				zoomSpeed -= zoomAcc * info.timeStep;
				if (zoomSpeed < 0)
					zoomSpeed = 0;
			} else if (zoomSpeed < 0) {
				zoomSpeed += zoomAcc * info.timeStep;
				if (zoomSpeed > 0)
					zoomSpeed = 0;
			}
			float minZoom = 1.3;
			zoom += zoomSpeed * info.timeStep;
			if (zoom < 0) {
				zoom = 0;
				zoomSpeed = 0;
			} else if (zoom != 0 && zoom < minZoom) {
				zoom = minZoom;
			}
			float scroll = IsScrolledY();
			if (scroll) {
				if (zoom == minZoom)
					zoom = 0;
				zoomSpeed -= scroll * (0.8 + zoom * 0.3);
			}
		}

		//Dimension* dim = app->getActiveSession()->getDimensions()[0];
		EngineObject* plr = requestPlayer();
		if (!plr) return;

		// death logic
		CombatData* combatData = GetCombatData(app->getActiveSession(), plr);
		deathTime -= info.timeStep;
		//printf("%f\n", deathTime);
		if (deathTime < 0) {
			deathTime = 0;
			if (isDead()) {
				setDead(false);
				plr->getWorld()->lockPhysics();
				auto tr = plr->getRigidBody()->getForce();
				auto br = plr->getRigidBody()->getTorque();
				plr->getRigidBody()->setLinearVelocity({ 0,0,0 });
				plr->getRigidBody()->setAngularVelocity({ 0,0,0 });
				plr->getRigidBody()->setAngularLockAxisFactor({0,1,0}); // only allow spin (y rotation)
				plr->getRigidBody()->setTransform({}); // reset position
				plr->getWorld()->unlockPhysics();
				combatData->health = combatData->getMaxHealth();
				app->getActiveSession()->netSetHealth(plr, combatData->health);
			}
		}
		if (!isDead() && combatData->health == 0) {
			// kill player
			deathTime = 5; // respawn time

			setDead(true);
			zoom = 4;

			setFlight(false);
			
			plr->getWorld()->lockPhysics();
			// make player jump
			plr->getRigidBody()->setAngularLockAxisFactor({1,1,1});

			rp3d::Vector3 randomTorque = { (float)GetRandom() - 0.5f,(float)GetRandom() - 0.5f,(float)GetRandom() - 0.5f };
			randomTorque *= deathShockStrength * 0.1f; // don't want to spin so much
			//randomTorque /= info.timeStep * info.timeStep;
			plr->getRigidBody()->applyLocalTorque(randomTorque);

			rp3d::Vector3 randomForce = { (float)GetRandom() - 0.5f,(float)GetRandom() / 2.f,(float)GetRandom() - 0.5f };
			randomForce *= deathShockStrength;
			//randomForce /= info.timeStep * info.timeStep;
			plr->getRigidBody()->applyWorldForceAtCenterOfMass(randomForce);
			plr->getWorld()->unlockPhysics();

			if(!keepInventory)
				dropAllItems();
		}

		//log::out << "speed: " << zoomSpeed << " "<<zoom<<"\n";

		glm::vec3 flatMove{};
		if (glm::length(flatInput) != 0) {
			flatMove = speed * glm::normalize(camera->getFlatLookVector() * flatInput.z + camera->getRightVector() * flatInput.x);
		}
		glm::vec3 moveDir = flatMove;
		if (!isDead()) {
			if (flight) {
				if (IsKeybindingDown(KeyJump)) {
					moveDir.y += speed;
				}
				if (IsKeybindingDown(KeyCrouch)) {
					moveDir.y -= speed;
				}
			} else {
				if (IsKeybindingPressed(KeyJump)) {
					moveDir.y += jumpForce;
				}
				if (IsKeybindingPressed(KeyCrouch)) {
					moveDir.y -= jumpForce;
				}
			}
		}
		if (plr->getRigidBody() && !isDead()) {
			plr->getWorld()->lockPhysics();
			rp3d::Vector3 rot = ToEuler(plr->getRigidBody()->getTransform().getOrientation());
			float realY = rot.y;
			float wantY = realY;

			if (IsKeybindingDown(KeyMoveCamera) || zoom == 0) {
				wantY = camera->getRotation().y - glm::pi<float>();
			} else if (glm::length(flatMove) != 0) {
				wantY = std::atan2(flatMove.x, flatMove.z);
			}

			float dv = AngleDifference(wantY, realY) / (info.timeStep);
			dv *= 0.3; // snap speed
			float velRotY = dv - plr->getRigidBody()->getAngularVelocity().y;
			// inertia tensor needs to be accounted for when applying torque.

			//rp3d::Vector3 newVel = rigidBody->getAngularVelocity();
			//newVel.y += velRotY;
			rp3d::Vector3 newVel = { 0,dv,0 };
			plr->getRigidBody()->setAngularVelocity(newVel);
			//log::out << "realY: "<<realY << " wantedY: "<<wantedY<<" diff: "<<diff<< "\n";
			//log::out <<"angY: "<<rigidBody->getAngularVelocity().y << "\n";
			//log::out << "velRoty: " << velRotY << " divDiff: "<<(diff / info.timeStep) << "\n";

			glm::vec3 bodyVel = ToGlmVec3(plr->getRigidBody()->getLinearVelocity());
			float keepY = bodyVel.y;
			float moveDirY = moveDir.y;
			if (!flight) {
				moveDir.y = 0;
				bodyVel.y = 0;
			}
			glm::vec3 flatVelDiff = moveDir - bodyVel;
			bool dontMove = false;
			if (!plr->getRigidBody()->isGravityEnabled() && glm::length(moveDir) == 0) {
				dontMove = true;
			}
			if (!dontMove) {
				float tolerance = 0.0001;
				if (glm::length(flatVelDiff) < tolerance && !flight) {
					glm::vec3 other{};
					if (!flight)
						other.y = keepY + moveDirY;
					plr->getRigidBody()->setLinearVelocity(ToRp3dVec3(other));
				} else {
					flatVelDiff *= 0.25;
					if (!flight)
						flatVelDiff.y = moveDirY;
					flatVelDiff /= info.timeStep;
					plr->getRigidBody()->applyWorldForceAtCenterOfMass(ToRp3dVec3(flatVelDiff));
				}
			}
			if (!flight) {
				rp3d::Vector3 extraDownForce = { 0,0,0 };
				float mass = plr->getRigidBody()->getMass();
				extraDownForce.y += -0.34 * 9.82 * mass;
				if (plr->getRigidBody()->getLinearVelocity().y < 0) {
					rp3d::Vector3 force = { 0,0,0 };
					extraDownForce.y += -1.2 * 9.82 * mass;
				}
				plr->getRigidBody()->applyLocalForceAtCenterOfMass(extraDownForce);
			}
			plr->getWorld()->unlockPhysics();
		}
		releasePlayer(plr);
	}
}