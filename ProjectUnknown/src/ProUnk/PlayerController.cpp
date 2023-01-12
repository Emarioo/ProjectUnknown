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
	void PlayerController::setPlayerObject(engone::EngineObject* player) {
		m_player = player;

		setFlight(true);
	}
	engone::EngineObject* PlayerController::requestPlayer() {
		Dimension* dim = app->getActiveSession()->getDimension("0");
		engone::EngineObject* plr = dim->getWorld()->requestAccess(m_playerId);
		return plr;
	}
	void PlayerController::releasePlayer(engone::EngineObject* plr) {
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
		}
		releasePlayer(plr);
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

		EngineObject* plr = getPlayerObject();
		if (!plr)
			return;
		// first slot in inventory is the held weapon.
		auto& playerInfo = app->getActiveSession()->objectInfoRegistry.getCreatureInfo(plr->getObjectInfo());
		Inventory* inv = app->getActiveSession()->inventoryRegistry.getInventory(playerInfo.inventoryId);

		Dimension* dim = app->getActiveSession()->getDimensions()[0];

		//EngineObject* heldObject = requestHeldObject();

		

		if (inv->getSlotSize() != 0) {
			auto& item = inv->getItem(0);
			//log::out << "Type: "<<item.getType() << "\n";
			if (lastItemType != item.getType()) {
				if (heldObject) {
					if (item.getType() == 0) {
						// remove held object
						DeleteObject(dim,heldObject);
						heldObject = nullptr;
					} else {
						// reuse held object
						heldObject->setModel(dim->getParent()->modelRegistry.getModel(item.getModelId()));
						heldObject->setOnlyTrigger(true);
					}
				} else {
					if (item.getType() != 0) {
						// create new object
						heldObject = CreateWeapon(dim, item);
						heldObject->setOnlyTrigger(true);
						//releaseHeldObject(heldObject);
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
			
			return;
		}
		if (m_player->getFlags() & OBJECT_IS_DEAD) {
			heldObject->setOnlyTrigger(false);
			return;
		}
		heldObject->setOnlyTrigger(true);

		ModelAsset* model = m_player->getModel();
		rp3d::RigidBody* body = m_player->getRigidBody();

		if (!model->valid())
			return;
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
			std::vector<glm::mat4> boneMats = model->getArmatureTransforms(m_player->getAnimator(), instMat, inst, arm, &baseBoneMats);
			Bone& lastBone = arm->bones.back();

			glm::mat4 gripMat = baseBoneMats.back();
			swordTrans = ToTransform(modelMatrix * gripMat);
		}

		heldObject->getRigidBody()->setTransform(swordTrans);

		heldObject->getRigidBody()->setAngularVelocity(body->getAngularVelocity());
		heldObject->getRigidBody()->setLinearVelocity(body->getLinearVelocity());
		heldObject->getRigidBody()->resetForce();
		heldObject->getRigidBody()->resetTorque();

		//heldWeapon->rigidBody->enableGravity(rigidBody->isGravityEnabled());
		//weaponState.sample(heldWeapon);
	}
	void PlayerController::setFlight(bool yes) {
		flight = yes;
		rp3d::RigidBody* body = m_player->getRigidBody();
		body->enableGravity(!yes);
		if (!yes)
			body->setLinearDamping(0.f);
		else
			body->setLinearDamping(7.f);
	}
	void PlayerController::setNoClip(bool yes) {
		noclip = yes;
		m_player->setOnlyTrigger(yes);
	}
	//void PlayerController::setWeapon(engone::EngineObject* weapon) {
	//	using namespace engone;
	//	if (heldObject) {
	//		heldObject->setOnlyTrigger(false);
	//		m_world->netEditObject(heldObject->getUUID(), 0, false);
	//		//heldWeapon->rigidBody->enableGravity(weaponState.gravity);
	//		//heldWeapon->rigidBody->setLinearVelocity(ToRp3dVec3(weaponState.sampledVelocity()));
	//		//weaponState.reset();
	//		SetCombatData(m_player, heldObject, false);
	//		m_world->netEditCombatData(heldObject->getUUID(), m_player->getUUID(), false);
	//		//if (heldWeapon->rigidBody->getNbColliders() != 0) {
	//		//	heldWeapon->rigidBody->getCollider(0)->setUserData(0);
	//		//	heldWeapon->flags &= ~OBJECT_HAS_COMBATDATA;
	//		//	heldWeapon->userData = nullptr;
	//		//}
	//		//	//ground->netEditObject(heldWeapon->getUUID(), 1, 0, );
	//		heldObject = nullptr;
	//	}
	//	if (!weapon) return; // we just wanted to get rid of the weapon.
	//	//weaponState.reset();
	//	// destroy joint to previous weapon.
	//	heldObject = weapon;
	//	// collider should exist for now
	//	SetCombatData(m_player, heldObject, true);
	//	m_world->netEditCombatData(heldObject->getUUID(), m_player->getUUID(), true);
	//	//if (heldWeapon->rigidBody->getNbColliders() != 0) {
	//	//	heldWeapon->rigidBody->getCollider(0)->setUserData((void*)COLLIDER_IS_DAMAGE);
	//	//	heldWeapon->flags |= OBJECT_HAS_COMBATDATA;
	//	//	heldWeapon->userData = userData;
	//	//	//ground->netEditObject(heldWeapon->getUUID(), 1, 0, getUUID().data[0], getUUID().data[1], 0);
	//	//	
	//	//}
	//	//weaponState.gravity = weapon->rigidBody->isGravityEnabled();
	//	std::vector<glm::mat4> transforms = m_player->getModel()->getParentTransforms(nullptr);
	//	rp3d::Vector3 anchor;
	//	rp3d::Transform swordTrans;
	//	glm::mat4 modelMatrix = ToMatrix(m_player->getRigidBody()->getTransform());
	//	AssetInstance* inst = nullptr;
	//	ArmatureAsset* arm = nullptr;
	//	glm::mat4 instMat;
	//	for (int i = 0; i < m_player->getModel()->instances.size(); i++) {
	//		auto& instance = m_player->getModel()->instances[i];
	//		if (instance.asset->type == ArmatureAsset::TYPE) {
	//			inst = &instance;
	//			arm = (ArmatureAsset*)inst->asset;
	//			instMat = transforms[i] * inst->localMat;
	//		}
	//	}
	//	if (inst != nullptr) {
	//		std::vector<glm::mat4> baseBoneMats;
	//		std::vector<glm::mat4> boneMats = m_player->getModel()->getArmatureTransforms(m_player->getAnimator(), instMat, inst, arm, &baseBoneMats);
	//		Bone& lastBone = arm->bones.back();
	//		glm::mat4 gripMat = baseBoneMats.back();
	//		anchor = ToRp3dVec3(gripMat[3]);
	//		swordTrans = ToTransform(modelMatrix * gripMat);
	//	}
	//	rp3d::Vector3 baseVec;
	//	weapon->setOnlyTrigger(true);
	//	m_world->netEditObject(weapon->getUUID(), 0, true);
	//	//heldWeapon->rigidBody->setAngularVelocity(rigidBody->getAngularVelocity());
	//	//heldWeapon->rigidBody->setLinearVelocity(rigidBody->getLinearVelocity());
	//	//weapon->rigidBody->resetForce();
	//	//weapon->rigidBody->resetTorque();
	//	//weapon->rigidBody->setTransform(swordTrans);
	//}
	//rp3d::decimal PlayerController::notifyRaycastHit(const rp3d::RaycastInfo& raycastInfo) {
	//	using namespace engone;
	//	// check if item
	//	EngineObject* object = (EngineObject*)raycastInfo.body->getUserData();
	//	if (!object)
	//		return 1.0;
	//	if (object->getObjectType() != OBJECT_ITEM)
	//		return 1.0;
	//	auto& itemInfo = app->getActiveSession()->objectInfoRegistry.getItemInfo(object->getObjectInfo());
	//	if (!itemInfo.item.getType())
	//		return 1.0;
	//	auto& dims = app->getActiveSession()->getDimensions();
	//	if (dims.size() == 0)
	//		return 1.0;
	//	auto& playerInfo = app->getActiveSession()->objectInfoRegistry.getCreatureInfo(m_player->getObjectInfo());
	//	Inventory* inv = app->getActiveSession()->inventoryRegistry.getInventory(playerInfo.inventoryId);
	//	bool transferred = inv->transferItem(itemInfo.item);
	//	
	//	if (transferred) {
	//		// delete item object if succefully added to inventory
	//		Dimension* dim = dims[0]; // Todo: the dimension should be the one the player is in.
	//		EngineWorld* world = dim->getWorld();
	//		DeleteObject(dim, object); // careful with memory leaks. functions is not complete
	//	}
	//	return 0.0; // we are done with raycast.
	//}
	engone::EngineObject* PlayerController::getSeenObject() {
		using namespace engone;
		auto& playerInfo = app->getActiveSession()->objectInfoRegistry.getCreatureInfo(m_player->getObjectInfo());
		Inventory* inv = app->getActiveSession()->inventoryRegistry.getInventory(playerInfo.inventoryId);
		CommonRenderer* renderer = GET_COMMON_RENDERER();
		if (!renderer) {
			log::out << log::RED << "PlayerController::getSeenObject : renderer is null\n";
			return nullptr;
		}
		Camera* camera = renderer->getCamera();

		auto& dims = app->getActiveSession()->getDimensions();
		if (dims.size() == 0)
			return nullptr;
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

		auto& playerInfo = app->getActiveSession()->objectInfoRegistry.getCreatureInfo(m_player->getObjectInfo());
		Inventory* inv = app->getActiveSession()->inventoryRegistry.getInventory(playerInfo.inventoryId);

		bool transferred = inv->transferItem(itemInfo.item);

		if (!transferred)
			return false;

		// delete item object if succefully added to inventory
		DeleteObject(dim, object); // careful with memory leaks. functions is not complete
		return true;		
	}
	Inventory* PlayerController::getInventory() {
		using namespace engone;
		auto& playerInfo = app->getActiveSession()->objectInfoRegistry.getCreatureInfo(m_player->getObjectInfo());
		Inventory* inv = app->getActiveSession()->inventoryRegistry.getInventory(playerInfo.inventoryId);
		return inv;
	}
	void PlayerController::dropItem(int slotIndex) {
		using namespace engone;
		auto& playerInfo = app->getActiveSession()->objectInfoRegistry.getCreatureInfo(m_player->getObjectInfo());
		Inventory* inv = app->getActiveSession()->inventoryRegistry.getInventory(playerInfo.inventoryId);

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
			object->setPosition(m_player->getPosition());
		}
		releaseHeldObject(heldObject);
	}
	void PlayerController::dropAllItems() {
		using namespace engone;
		auto& playerInfo = app->getActiveSession()->objectInfoRegistry.getCreatureInfo(m_player->getObjectInfo());
		Inventory* inv = app->getActiveSession()->inventoryRegistry.getInventory(playerInfo.inventoryId);

		auto& dims = app->getActiveSession()->getDimensions();
		if (dims.size() == 0)
			return;
		Dimension* dim = dims[0]; // Todo: the dimension should be the one the player is in.
		auto heldObject = requestHeldObject();
		if (heldObject) {
			if (inv->getSlotSize() != 0) {
				Item& item = inv->getItem(0);
			
				auto object = CreateItem(dim, item);
				item = Item();

				// copy velocity as well?
				object->getRigidBody()->setTransform(heldObject->getRigidBody()->getTransform());

				rp3d::Vector3 force = { (float)GetRandom() - .5f, (float)GetRandom(),(float)GetRandom() - .5f };
				force *= 600.f;
				object->getRigidBody()->applyWorldForceAtCenterOfMass(force);

				rp3d::Vector3 torque = { (float)GetRandom() - .5f, (float)GetRandom(),(float)GetRandom() - .5f };
				torque *= 600.f;
				object->getRigidBody()->applyWorldForceAtCenterOfMass(torque);

			}
		}
		releaseHeldObject(heldObject);
		for (int i = 0; i < inv->getSlotSize();i++) {
			Item& item = inv->getItem(i);
			if (item.getType() == 0)
				continue;

			auto object = CreateItem(dim, item);
			item = Item(); // clear slot

			glm::vec3 offset = { GetRandom() - .5f, 1 + GetRandom()-.5f,GetRandom()-.5f};
			offset *= .5;
			object->setPosition(m_player->getPosition()+offset);

			rp3d::Vector3 force = { (float)GetRandom() - .5f, (float)GetRandom(),(float)GetRandom() - .5f };
			force *= 600.f;
			object->getRigidBody()->applyWorldForceAtCenterOfMass(force);

			rp3d::Vector3 torque = { (float)GetRandom() - .5f, (float)GetRandom(),(float)GetRandom() - .5f };
			torque *= 600.f;
			object->getRigidBody()->applyWorldForceAtCenterOfMass(torque);
		}
	}
	void PlayerController::performSkills(engone::LoopInfo& info) {
		using namespace engone;
		CombatData* weaponCombat = nullptr;
		//auto heldObject = requestHeldObject();
		if (heldObject)
			weaponCombat = GetCombatData(app->getActiveSession(), heldObject);

		if (!weaponCombat)
			return;

		weaponCombat->update(info);
		//releaseHeldObject(heldObject);
		//log::out << "frame: " <<m_player->getAnimator()->enabledAnimations[0].frame << "\n";

		if (IsKeyDown(GLFW_MOUSE_BUTTON_LEFT)) {
			if (!weaponCombat->attacking) {
				// Set damage of weapon
				weaponCombat->damageType = CombatData::SINGLE_DAMAGE;
				Inventory* inv = getInventory();
				Item& firstItem = inv->getItem(0);


				ComplexData* complexData = app->getActiveSession()->complexDataRegistry.getData(firstItem.getComplexData());
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
					m_player->getAnimator()->enable("Player", "PlayerDownSwing", { false,1,1,0 });
					//app->getActiveSession()->netAnimateObject(m_player, "Player", "PlayerDownSwing", false, 1, 1, 0);
					AnimatorProperty* prop = m_player->getAnimator()->getProp("Player");
					if (prop) {
						weaponCombat->animationTime = prop->getRemainingSeconds();
					}
				}
			}
		}
		if (IsKeyDown(GLFW_KEY_F)) {
			if (!weaponCombat->attacking) {
				// Set damage of weapon
				weaponCombat->damageType = CombatData::SINGLE_DAMAGE;
				Inventory* inv = getInventory();
				Item& firstItem = inv->getItem(0);

				ComplexData* complexData = app->getActiveSession()->complexDataRegistry.getData(firstItem.getComplexData());
				ComplexPropertyType* atkProperty = app->getActiveSession()->complexDataRegistry.getProperty("atk");
				ComplexPropertyType* knockProperty = app->getActiveSession()->complexDataRegistry.getProperty("knock");
				weaponCombat->singleDamage = complexData->get(atkProperty);
				weaponCombat->knockStrength = complexData->get(knockProperty); // side knock?

				weaponCombat->singleDamage *= 1.1f; // amplify damage a little with this skill

				// other stuff
				weaponCombat->skillType = SKILL_SIDE_SLASH;
				weaponCombat->attack();
				m_player->getAnimator()->enable("Player", "PlayerSideSwing", { false,1,1,0 });
				//app->getActiveSession()->netAnimateObject(m_player, "Player", "PlayerSideSwing", false, 1, 1, 0);
				AnimatorProperty* prop = m_player->getAnimator()->getProp("Player");
				if (prop) {
					weaponCombat->animationTime = prop->getRemainingSeconds();
				}
			}
		}
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
		if (IsKeyPressed(GLFW_KEY_E)) {
			pickupItem(obj);
		}
		if (IsKeyPressed(GLFW_KEY_Q)) {
			dropItem(0);
		}
		//Dimension* dim = app->getActiveSession()->getDimensions()[0];

		CombatData* playerCombat = GetCombatData(app->getActiveSession(), m_player);
		if (IsKeyPressed(GLFW_KEY_R)) {
			if (isDead()) {
				deathTime = 0;
				setDead(false);
				m_player->getRigidBody()->setAngularLockAxisFactor({0,1,0});
				//auto tr = rigidBody->getTransform(); // only reset orientation
				//tr.setOrientation({ 0,0,0,1.f });
				//rigidBody->setTransform(tr);
				m_player->getRigidBody()->setTransform({}); // reset position too

				playerCombat->health = playerCombat->getMaxHealth();
			}
		}
		if (IsKeyPressed(GLFW_KEY_K)) {
			playerCombat->health = 0; // kill player
		}
		if (IsKeyPressed(GLFW_KEY_K)) {
			playerCombat->health = 0;
		}
		performSkills(info);
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
		
		if (m_player->getRigidBody() && !isDead()) {
			glm::vec3 pos = m_player->getPosition();
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

		// death logic
		CombatData* combatData = GetCombatData(app->getActiveSession(), m_player);
		deathTime -= info.timeStep;
		//printf("%f\n", deathTime);
		if (deathTime < 0) {
			deathTime = 0;
			if (isDead()) {
				setDead(false);
				auto tr = m_player->getRigidBody()->getForce();
				auto br = m_player->getRigidBody()->getTorque();
				m_player->getRigidBody()->setLinearVelocity({ 0,0,0 });
				m_player->getRigidBody()->setAngularVelocity({ 0,0,0 });
				m_player->getRigidBody()->setAngularLockAxisFactor({0,1,0}); // only allow spin (y rotation)
				m_player->getRigidBody()->setTransform({}); // reset position
				combatData->health = combatData->getMaxHealth();
			}
		}
		if (!isDead() && combatData->health == 0) {
			// kill player
			deathTime = 5;

			setDead(true);
			zoom = 4;

			setFlight(false);
			
			// make player jump
			m_player->getRigidBody()->setAngularLockAxisFactor({1,1,1}); // only allow spin (y rotation)

			rp3d::Vector3 randomTorque = { (float)GetRandom() - 0.5f,(float)GetRandom() - 0.5f,(float)GetRandom() - 0.5f };
			randomTorque *= deathShockStrength * 0.1f; // don't want to spin so much
			//randomTorque /= info.timeStep * info.timeStep;
			m_player->getRigidBody()->applyLocalTorque(randomTorque);

			rp3d::Vector3 randomForce = { (float)GetRandom() - 0.5f,(float)GetRandom() / 2.f,(float)GetRandom() - 0.5f };
			randomForce *= deathShockStrength;
			//randomForce /= info.timeStep * info.timeStep;
			m_player->getRigidBody()->applyWorldForceAtCenterOfMass(randomForce);

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
		if (m_player->getRigidBody() && !isDead()) {
			rp3d::Vector3 rot = ToEuler(m_player->getRigidBody()->getTransform().getOrientation());
			float realY = rot.y;
			float wantY = realY;

			if (IsKeybindingDown(KeyMoveCamera) || zoom == 0) {
				wantY = camera->getRotation().y - glm::pi<float>();
			} else if (glm::length(flatMove) != 0) {
				wantY = std::atan2(flatMove.x, flatMove.z);
			}

			float dv = AngleDifference(wantY, realY) / (info.timeStep);
			dv *= 0.3; // snap speed
			float velRotY = dv - m_player->getRigidBody()->getAngularVelocity().y;
			// inertia tensor needs to be accounted for when applying torque.

			//rp3d::Vector3 newVel = rigidBody->getAngularVelocity();
			//newVel.y += velRotY;
			rp3d::Vector3 newVel = { 0,dv,0 };
			m_player->getRigidBody()->setAngularVelocity(newVel);

			//log::out << "realY: "<<realY << " wantedY: "<<wantedY<<" diff: "<<diff<< "\n";
			//log::out <<"angY: "<<rigidBody->getAngularVelocity().y << "\n";
			//log::out << "velRoty: " << velRotY << " divDiff: "<<(diff / info.timeStep) << "\n";

			glm::vec3 bodyVel = ToGlmVec3(m_player->getRigidBody()->getLinearVelocity());
			float keepY = bodyVel.y;
			float moveDirY = moveDir.y;
			if (!flight) {
				moveDir.y = 0;
				bodyVel.y = 0;
			}
			glm::vec3 flatVelDiff = moveDir - bodyVel;
			bool dontMove = false;
			if (!m_player->getRigidBody()->isGravityEnabled() && glm::length(moveDir) == 0) {
				dontMove = true;
			}
			if (!dontMove) {
				float tolerance = 0.0001;
				if (glm::length(flatVelDiff) < tolerance && !flight) {
					glm::vec3 other{};
					if (!flight)
						other.y = keepY + moveDirY;
					m_player->getRigidBody()->setLinearVelocity(ToRp3dVec3(other));
				} else {
					flatVelDiff *= 0.25;
					if (!flight)
						flatVelDiff.y = moveDirY;
					flatVelDiff /= info.timeStep;
					m_player->getRigidBody()->applyWorldForceAtCenterOfMass(ToRp3dVec3(flatVelDiff));
				}
			}
			if (!flight) {
				rp3d::Vector3 extraDownForce = { 0,0,0 };
				float mass = m_player->getRigidBody()->getMass();
				extraDownForce.y += -0.34 * 9.82 * mass;
				if (m_player->getRigidBody()->getLinearVelocity().y < 0) {
					rp3d::Vector3 force = { 0,0,0 };
					extraDownForce.y += -1.2 * 9.82 * mass;
				}
				m_player->getRigidBody()->applyLocalForceAtCenterOfMass(extraDownForce);
			}
		}
	}
}