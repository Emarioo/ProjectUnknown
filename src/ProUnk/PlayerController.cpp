#include "ProUnk/PlayerController.h"

#include "Engone/EventModule.h"
#include "ProUnk/Keybindings.h"
#include "Engone/Window.h"

// #include "ProUnk/GameApp.h"

#include "ProUnk/Combat/Combat.h"

#include "ProUnk/Objects/BasicObjects.h"

namespace prounk {
	PlayerController::PlayerController() {
		
	}
	void PlayerController::setPlayerObject(engone::UUID player) {
		playerId = player;

		// setFlight(true);
	}
	void PlayerController::update(engone::LoopInfo* info, engone::EngineObject* player) {
		using namespace engone;
		Movement(info,player);
		
		DetectHeldWeapon(info,player);
		UpdateWeaponTransform(info,player);
		Input(info,player);
	}
	void PlayerController::DetectHeldWeapon(engone::LoopInfo* info, engone::EngineObject* plr) {
		using namespace engone;
		if (!plr)
			return;
		// first slot in inventory is the held weapon.
		Inventory* inv = GetInventory(plr);
		if (!inv)
			return;
			
		Dimension* dim = (Dimension*)plr->getWorld()->getUserData();
		Session* session = dim->getParent();

		EngineObject* heldObject = GetHeldObject(plr);

		if (inv->getSlotSize() != 0) {
			auto item = inv->getItem(0);
			if (lastItemType != item->getType()) {
				if (heldObject) {
					if (item->getType() == 0) {
						// remove held object
						session->netDeleteObject(heldObject);
						DeleteObject(dim, heldObject);
						SetHeldObject(plr,0);
						heldObject = nullptr;
					} else {
						if (!IsDead(plr)) {
							// reuse held object
							ModelAsset* model = session->modelRegistry.getModel(item->getModelId());
							heldObject->setModel(model);
							heldObject->setOnlyTrigger(true);
							session->netEditTrigger(heldObject, true);
							session->netEditModel(heldObject, model->getLoadName());
						}
					}
				} else {
					if (item->getType() != 0) {

						if (!IsDead(plr)) {
							// create new object
							ModelAsset* model = session->modelRegistry.getModel(item->getModelId());
							heldObject = CreateWeapon((Dimension*)plr->getWorld()->getUserData(), model->getLoadName());
							SetHeldObject(plr,heldObject);
							heldObject->setOnlyTrigger(true);
							GetCombatData(heldObject)->owner = plr;

							session->netAddGeneral(heldObject);
							session->netEditTrigger(heldObject, true);
							session->netMoveObject(heldObject);
						}
					}
					// damage in combat data is set when doing a skill
				}
				lastItemType = item->getType();
			} else {
				// do nothing if types are the same
			}
		}
	}
	void PlayerController::UpdateWeaponTransform(engone::LoopInfo* info, engone::EngineObject* plr) {
		using namespace engone;
		if (!plr)
			return;
		// first slot in inventory is the held weapon.
		Inventory* inv = GetInventory(plr);
		Dimension* dim = (Dimension*)plr->getWorld()->getUserData();
		Session* session = dim->getParent();

		EngineObject* heldObject = GetHeldObject(plr);

		if (!heldObject) {
			//releasePlayer(plr);
			return;
		}

		if (IsDead(plr)) {
			if (heldObject->isOnlyTrigger()) {
				heldObject->setOnlyTrigger(false);
				session->netEditTrigger(heldObject, false);
			}
			return;
		}
		if (!heldObject->isOnlyTrigger()) {
			heldObject->setOnlyTrigger(true);
			session->netEditTrigger(heldObject, true);
		}

		ModelAsset* model = plr->getModel();
		rp3d::RigidBody* body = plr->getRigidBody();

		if (!model->valid()) {
			//releasePlayer(plr);
			//releaseHeldObject(heldObject);
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

		heldObject->getWorld()->lockPhysics();
		heldObject->getRigidBody()->setTransform(swordTrans);
		heldObject->getRigidBody()->setAngularVelocity(body->getAngularVelocity());
		heldObject->getRigidBody()->setLinearVelocity(body->getLinearVelocity());
		heldObject->getRigidBody()->applyWorldForceAtCenterOfMass(body->getForce());
		heldObject->getRigidBody()->applyWorldTorque(body->getTorque());
		heldObject->getWorld()->unlockPhysics();

		// cleanup
		//releasePlayer(plr);
		//releaseHeldObject(heldObject);
		//weaponState.sample(heldWeapon);
	}
	void PlayerController::setFlight(bool yes, engone::EngineObject* plr) {
		using namespace engone;
		// EngineObject* plr = requestPlayer();
		if (!plr) return;

		flight = yes;
		plr->getWorld()->lockPhysics();
		
		plr->getRigidBody()->setIsSleeping(false);
		plr->getRigidBody()->enableGravity(!yes);
		plr->getRigidBody()->setLinearDamping(yes? 7.0 : 0.f);

		plr->getWorld()->unlockPhysics();
	}
	void PlayerController::setNoClip(bool yes, engone::EngineObject* plr) {
		using namespace engone;
		noclip = yes;
		if (!plr) return;
		plr->setOnlyTrigger(yes);
	}
	
	engone::EngineObject* PlayerController::getSeenObject(engone::LoopInfo* info, engone::EngineObject* player) {
		using namespace engone;

		CommonRenderer* renderer = info->window->getCommonRenderer();
		
		if (!renderer) {
			log::out << log::RED << "PlayerController::getSeenObject : renderer is null\n";
			return nullptr;
		}
		Camera* camera = renderer->getCamera();
		
		Dimension* dim = (Dimension*)player->getWorld()->getUserData();
		Session* session = dim->getParent();

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
	bool PlayerController::pickupItem(engone::EngineObject* plr, engone::EngineObject* object) {
		using namespace engone;
		
		// TODO: add log messages?

		if (!object)
			return false;
		if (object->getObjectType() != OBJECT_ITEM)
			return false;

		Dimension* dim = (Dimension*)plr->getWorld()->getUserData();
		Session* session = dim->getParent();

		EngineWorld* world = dim->getWorld();
		
		auto itemInfo = session->objectInfoRegistry.getItemInfo(object->getObjectInfo());
		if (!itemInfo->item.getType())
			return false;

		Inventory* inv = GetInventory(plr);

		bool transferred = inv->transferItem(&itemInfo->item);
		//int slot = inv->findAvailableSlot(iteminfo->item);
		//if (slot >= 0)
		//	iteminfo->item.copy(inv->getItem(slot));

		if (!transferred)
			return false;

		//log::out << "pickup "<<object->getUUID() << "\n";

		// delete item object if succefully added to inventory
		session->netDeleteObject(object);
		DeleteObject(dim, object); // careful with memory leaks. functions is not complete
		return true;
	}
	void PlayerController::dropItem(int slotIndex, int count, engone::EngineObject* player) {
		using namespace engone;
		Inventory* inv = GetInventory(player);
		if (count <= 0) return;

		if (inv->getSlotSize() <=slotIndex)
			return;

		Item* item = inv->getItem(slotIndex);
		if (item->getType() == 0)
			return;
	
		Dimension* dim = (Dimension*)player->getWorld()->getUserData();
		Session* session = dim->getParent();

		engone::EngineObject* object = nullptr;
		//if (item.getCount() > count) {
		//	Item copyItem;
		//	item.transfer(copyItem,count);
		//	object = CreateItem(dim, copyItem);
		//} else {
		Item copyItem;
		item->transfer(copyItem,count);
		object = CreateItem(dim, copyItem);
		
		// held object is deleted automatically later if item is zero
		auto heldObject = GetHeldObject(player);
		if (slotIndex==0&&heldObject) {
			object->getRigidBody()->setTransform(heldObject->getRigidBody()->getTransform());
		} else {
			object->setPosition(player->getPosition());
		}

		session->netAddGeneral(object);
		session->netMoveObject(object);
	}
	void PlayerController::dropAllItems(engone::EngineObject* plr) {
		using namespace engone;

		if (!plr) return;
		Inventory* inv = GetInventory(plr);

		Dimension* dim = (Dimension*)plr->getWorld()->getUserData();
		Session* session = dim->getParent();


		// Drops the held object at a specific position
		
		auto heldObject = GetHeldObject(plr);
		for (int i = 0; i < inv->getSlotSize();i++) {
			Item* item = inv->getItem(i);

			if (item->getType() == 0)
				continue;

			auto object = CreateItem(dim, *item);
			*item = Item(); // clear slot

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

			session->netAddGeneral(object);
			session->netMoveObject(object);
		}
	}
	void PlayerController::performSkills(engone::LoopInfo* info, engone::EngineObject* plr) {
		using namespace engone;
		if (!plr) return;
		
		CombatData* weaponCombat = nullptr;
		auto heldObject = GetHeldObject(plr);
		if (!heldObject) {
			return;
		}
			
		weaponCombat = GetCombatData(heldObject);
		if (!weaponCombat) {
			return;
		}

		Dimension* dim = (Dimension*)plr->getWorld()->getUserData();
		Session* session = dim->getParent();
		//releaseHeldObject(heldObject);
		//log::out << "frame: " <<m_player->getAnimator()->enabledAnimations[0].frame << "\n";
		if (IsKeyDown(GLFW_MOUSE_BUTTON_LEFT)) {
			if (!weaponCombat->attacking) {
				// Set damage of weapon
				weaponCombat->damageType = CombatData::SINGLE_DAMAGE;
				Inventory* inv = GetInventory(plr);
				Item* firstItem = inv->getItem(0);
				
				auto spear = session->itemTypeRegistry.getType("spear");
				auto sword = session->itemTypeRegistry.getType("sword");
				auto dagger = session->itemTypeRegistry.getType("dagger");

				ComplexData* complexData = firstItem->getComplexData();
				//ComplexData* complexData = app->getActiveSession()->complexDataRegistry.getData(firstItem.getComplexData());
				if (complexData) { // cannot attack without data
					ComplexPropertyType* atkProperty = session->complexDataRegistry.getProperty("atk");
					ComplexPropertyType* knockProperty = session->complexDataRegistry.getProperty("knock");
					weaponCombat->singleDamage = complexData->get(atkProperty);
					weaponCombat->knockStrength = complexData->get(knockProperty);

					weaponCombat->skillType = SKILL_SLASH; // irrelevant
					weaponCombat->attack();
					if (firstItem->getType()==sword->itemType) {
						plr->getAnimator()->enable("Player", "PlayerSwordSlash", { false,1,1,0 });
						session->netAnimateObject(plr, "Player", "PlayerSwordSlash", false, 1, 1, 0);
					}else if (firstItem->getType() == dagger->itemType) {
						plr->getAnimator()->enable("Player", "PlayerDaggerThrust", { false,1,1,0 });
						session->netAnimateObject(plr, "Player", "PlayerDaggerThrust", false, 1, 1, 0);
					} else if (firstItem->getType() == spear->itemType) {
						plr->getAnimator()->enable("Player", "PlayerSpearThrust", { false,1,1,0 });
						session->netAnimateObject(plr, "Player", "PlayerSpearThrust", false, 1, 1, 0);
					}
					AnimatorProperty* prop = plr->getAnimator()->getProp("Player");
					if (prop) {
						weaponCombat->animationTime = prop->getRemainingSeconds();
					}
				} else {
					log::out << "PlayerController::performSkills : Weapon is missing complex data\n";
				}
			}
		}
	}
	void PlayerController::Input(engone::LoopInfo* info, engone::EngineObject* plr) {
		using namespace engone;
		if (IsKeyPressed(GLFW_KEY_G)) {
			setFlight(!flight,plr);
		}
		// if (IsKeyPressed(GLFW_KEY_T)) {
		// 	engone::EngineObject* plr = requestPlayer();
		// 	auto renderer = info->window->getCommonRenderer();
		// 	plr->setPosition(plr->getPosition() + renderer->getCamera()->getFlatLookVector()*5.f);
		// 	//plr->setPosition(plr->getPosition() + glm::vec3(0, -4, 0));
		// 	//plr->applyForce({0,100,0});
		// 	//setFlight(!flight);
		// }
		if (IsKeyPressed(GLFW_KEY_C)) {
			setNoClip(!noclip,plr);
		}
		Dimension* dim = (Dimension*)plr->getWorld()->getUserData();
		Session* session = dim->getParent();
		
		hoveredItem.clear();
		EngineObject* obj = getSeenObject(info,plr);
		if (obj) {
			if (obj->getObjectType() == OBJECT_ITEM) {
				auto oinfo = session->objectInfoRegistry.getItemInfo(obj->getObjectInfo());
				if (!oinfo) {
					log::out << log::RED << "PlayerController::Input : hovered item has null info\n";
				} else {
					hoveredItem = oinfo->item.getDisplayName();
				}
			}
		}
		//if (IsKeyPressed(GLFW_KEY_E)) {
		if (IsKeyDown(GLFW_KEY_E)) {
			pickupItem(obj,plr);
		}
		if (IsKeyPressed(GLFW_KEY_Q)) {
			dropItem(0,1,plr);
		}
		//Dimension* dim = app->getActiveSession()->getDimensions()[0];

		CombatData* playerCombat = GetCombatData(plr);
		if (IsKeyPressed(GLFW_KEY_R)) {
			if (IsDead(plr)) {
				deathTime = 0;
				info->window->enableFirstPerson(true);
				plr->getWorld()->lockPhysics();
				plr->getRigidBody()->setAngularLockAxisFactor({0,1,0});
				//auto tr = rigidBody->getTransform(); // only reset orientation
				//tr.setOrientation({ 0,0,0,1.f });
				//rigidBody->setTransform(tr);
				plr->getRigidBody()->setTransform({}); // reset position too
				plr->getWorld()->unlockPhysics();

				playerCombat->setHealth(playerCombat->getMaxHealth());
				session->netEditHealth(plr, playerCombat->getHealth());
			}
		}
		if (IsKeyPressed(GLFW_KEY_K)) {
			playerCombat->setHealth(0); // kill player
			session->netEditHealth(plr, playerCombat->getHealth());
		}
		if (IsKeyPressed(GLFW_KEY_L)) {
			playerCombat->setHealth(playerCombat->getMaxHealth());
			session->netEditHealth(plr, playerCombat->getHealth());
		}
		performSkills(info,plr);
	}
	void PlayerController::render(engone::LoopInfo* info, engone::EngineObject* plr) {
		using namespace engone;

		FontAsset* consolas = info->window->getStorage()->get<FontAsset>("fonts/consolas42");

		if (!hoveredItem.empty()) {
			ui::TextBox textBox = { hoveredItem, 0, 0, 30, consolas, {0,1.f,1.f,1.f}};
			textBox.x = info->window->getWidth() / 2 - textBox.getWidth()/2;
			textBox.y = info->window->getHeight() / 2 - textBox.getHeight()/2-50;
			ui::Draw(textBox);
		}
		if (!plr)
			return;
		if (plr->getRigidBody() && !IsDead(plr)) {
			//glm::vec3 pos = plr->getPosition();
			glm::mat4 mat = plr->getInterpolatedMat4(info->interpolation);
			CommonRenderer* renderer = info->window->getCommonRenderer();
			if (!renderer) {
				log::out << log::RED << "PlayerController::render : renderer is null\n";
			} else {
				Camera* cam = renderer->getCamera();
				// printf("%f\n",zoom);
				glm::vec3 camPos = glm::vec3(mat[3]) + camOffset - cam->getLookVector() * zoom;
				
				if (IsKeyDown(GLFW_MOUSE_BUTTON_2)&&zoom!=0) {
					offsetTransitionTime = Min(offsetTransitionTime +info->timeStep,offsetTransitionTimeMax);
				} else {
					offsetTransitionTime = Max(offsetTransitionTime - info->timeStep, 0);
				}
				//if(zoom!=0)
					camPos+=cam->getRightVector() * .9f * bezier(offsetTransitionTime,0, offsetTransitionTimeMax);
				cam->setPosition(camPos);
			}
		}
		else {
			//log::out << "DEAD\n";
		}
	}
#define Key_Forward GLFW_KEY_W
#define Key_Left GLFW_KEY_A
#define Key_Back GLFW_KEY_S
#define Key_Right GLFW_KEY_D
#define Key_Jump GLFW_KEY_SPACE
#define Key_Crouch GLFW_KEY_LEFT_CONTROL
#define Key_Sprint GLFW_KEY_LEFT_SHIFT
#define Key_MoveCamera GLFW_MOUSE_BUTTON_RIGHT
#define Key_Pause GLFW_KEY_ESCAPE
	void PlayerController::Movement(engone::LoopInfo* info, engone::EngineObject* plr) {
		using namespace engone;
		// using namespace prounk;

		// engone::EngineObject* plr = (EngineObject*)_plr;

		Window* window = info->window;
		if (!window) return;
		CommonRenderer* renderer = info->window->getCommonRenderer();
		//  GET_COMMON_RENDERER();
		//Renderer* renderer = window->getRenderer(); // if window is valid, render and camera should as well. BUT i may change somethings which would break this assumption. So i'll keep it.
		if (!renderer) return;
		Camera* camera = renderer->getCamera();
		if (!camera) return;

		float speed = walkSpeed;
		if (flight)
			speed = flySpeed;
		if (window->isKeyDown(Key_Sprint)) {
			if (flight)
				speed = flyFastSpeed;
			else
				speed = sprintSpeed;
		}

		Dimension* dim = (Dimension*)plr->getWorld()->getUserData();

		glm::vec3 flatInput = { 0,0,0 };
		if (window->isKeyDown(Key_Forward)) {
			flatInput.z += 1;
		}
		if (window->isKeyDown(Key_Left)) {
			flatInput.x -= 1;
		}
		if (window->isKeyDown(Key_Back)) {
			flatInput.z -= 1;
		}
		if (window->isKeyDown(Key_Right)) {
			flatInput.x += 1;
		}
		// EngineObject* plr = requestPlayer();
		if (!plr) return;
		if (!prounk::IsDead(plr)&&window->isCursorLocked()) {
			float zoomAcc = 0.1 + 5 * abs(zoomSpeed);
			if (zoomSpeed > 0) {
				zoomSpeed -= zoomAcc * info->timeStep;
				if (zoomSpeed < 0)
					zoomSpeed = 0;
			} else if (zoomSpeed < 0) {
				zoomSpeed += zoomAcc * info->timeStep;
				if (zoomSpeed > 0)
					zoomSpeed = 0;
			}
			float minZoom = 1.3;
			zoom += zoomSpeed * info->timeStep;
			if (zoom < 0) {
				zoom = 0;
				zoomSpeed = 0;
			} else if (zoom != 0 && zoom < minZoom) {
				zoom = minZoom;
			}
			float scroll = info->window->getScrollY();
			// log::out << "Scroll "<<scroll<<" "<<zoomSpeed<<"\n";
			if (scroll) {
				if (zoom == minZoom)
					zoom = 0;
				zoomSpeed -= scroll * (3 + zoom * 0.3);
			}
		}

		// death logic
		prounk::CombatData* combatData = prounk::GetCombatData(plr);
		if (combatData->getHealth() == 0 && deathTime == 0) {
			//log::out << "Kill player\n";
			// kill player
			deathTime = 5; // respawn time

			// PlayerSetFlight(plr,false);
			setFlight(false,plr);
			info->window->enableFirstPerson(false);

			plr->getWorld()->lockPhysics();
			// make player jump
			plr->getRigidBody()->setAngularLockAxisFactor({ 1,1,1 });

			rp3d::Vector3 randomTorque = { (float)GetRandom() - 0.5f,(float)GetRandom() - 0.5f,(float)GetRandom() - 0.5f };
			randomTorque *= deathShockStrength * 0.1f; // don't want to spin so much
			//randomTorque /= info->timeStep * info->timeStep;
			plr->getRigidBody()->applyLocalTorque(randomTorque);

			rp3d::Vector3 randomForce = { (float)GetRandom() - 0.5f,(float)GetRandom() / 2.f,(float)GetRandom() - 0.5f };
			randomForce *= deathShockStrength;
			//randomForce /= info->timeStep * info->timeStep;
			plr->getRigidBody()->applyWorldForceAtCenterOfMass(randomForce);
			plr->getWorld()->unlockPhysics();

			// PlayerDropInventory(plr);
			dropAllItems(plr);
			// if (!keepInventory)
				// dropAllItems();
		}

		deathTime -= info->timeStep;
		if (deathTime < 0) {
			deathTime = 0;
			if (prounk::IsDead(plr)) {
				//log::out << "Respawn player\n";
				info->window->enableFirstPerson(true);
				plr->getWorld()->lockPhysics();
				auto tr = plr->getRigidBody()->getForce();
				auto br = plr->getRigidBody()->getTorque();
				plr->getRigidBody()->setLinearVelocity({ 0,0,0 });
				plr->getRigidBody()->setAngularVelocity({ 0,0,0 });
				plr->getRigidBody()->setAngularLockAxisFactor({0,1,0}); // only allow spin (y rotation)
				plr->getRigidBody()->setTransform({}); // reset position
				plr->getWorld()->unlockPhysics();
				combatData->setHealth(combatData->getMaxHealth());
				dim->getParent()->netEditHealth(plr, combatData->getHealth());
			}
		}
		// Death camera
		if (combatData->getHealth() == 0) {
			glm::vec3 pos = plr->getPosition();
			CommonRenderer* renderer =  info->window->getCommonRenderer();
			if (!renderer) {
				log::out << log::RED << "PlayerController::render : renderer is null\n";
			} else {
				Camera* cam = renderer->getCamera();

				glm::vec3 diff = cam->getPosition() - pos;
				float length = glm::length(diff);
				glm::vec3 dir = diff/length;
				glm::vec3 rot{};
				rot.y = std::atan2(dir.x, dir.z);
				rot.x = std::asin(-dir.y);
				cam->setRotation(rot);
				// Could add some slight rotation to the camera. There is a chance the camera would go through
				// walls though.
				float maxLength = 10;
				if (length > maxLength) {
					cam->setPosition(dir* maxLength +pos);
				}
				//log::out << "CAM LOCK\n";
			}
		}

		glm::vec3 flatMove{};
		if (glm::length(flatInput) != 0) {
			flatMove = speed * glm::normalize(camera->getFlatLookVector() * flatInput.z + camera->getRightVector() * flatInput.x);
		}
		glm::vec3 moveDir = flatMove;
		jumpTime -= info->timeStep;
		if (jumpTime < 0)
			jumpTime = 0;
		if (!IsDead(plr)) {
			if (flight) {
				if (window->isKeyDown(Key_Jump)) {
					moveDir.y += speed;
				}
				if (window->isKeyDown(Key_Crouch)) {
					moveDir.y -= speed;
				}
			} else {
				if (window->isKeyDown(Key_Jump)) {
					auto oinfo = GetSession(plr)->objectInfoRegistry.getCreatureInfo(plr->getObjectInfo());
					if (oinfo->onGround) {
						if (jumpTime == 0) {
							moveDir.y += jumpForce;
							oinfo->onGround = false;
							jumpTime = jumpDelay;
						}
					}
				}
				if (window->isKeyPressed(Key_Crouch)) {
					moveDir.y -= jumpForce;
				}
			}
		}
		//return;
		if (plr->getRigidBody() && !IsDead(plr)) {
			plr->getWorld()->lockPhysics();
			rp3d::Vector3 rot = ToEuler(plr->getRigidBody()->getTransform().getOrientation());
			float realY = rot.y;
			float wantY = realY;

			float targetMargin = 0.06; // low margin is hard to achive because of friction.
			float targetSnapSpeed = 0.3;
			float targetDiff = fabs(AngleDifference(realY, targetRotation));
			//log::out << "diff " << targetDiff << "\n";
			if (targetDiff < targetMargin) {
				followTargetRotation = false;
			}

			if (window->isKeyDown(Key_MoveCamera) || zoom == 0) {
				targetRotation = camera->getRotation().y - glm::pi<float>();
				followTargetRotation = true;
			}

			if (followTargetRotation) {
				wantY = targetRotation;
			}else if (glm::length(flatMove) != 0) {
				wantY = std::atan2(flatMove.x, flatMove.z);
			}

			float dv = AngleDifference(wantY, realY) / (info->timeStep);
			//log::out << "a " << dv << "\n";
			//if (dv < 0.01) {
			//	rp3d::Transform t = plr->getRigidBody()->getTransform();
			//	t.setOrientation(rp3d::Quaternion::fromEulerAngles(rp3d::Vector3(0,wantY,0)));
			//	plr->getRigidBody()->setTransform(t);
			//} else {
				dv *= targetSnapSpeed;
				if (targetDiff < targetMargin * 2) // extra snap when close to margin so that the turning doesn't get stuck.
					dv *= 3;
				rp3d::Vector3 newVel = { 0,dv,0 };
				plr->getRigidBody()->setAngularVelocity(newVel);
			//}
			//log::out << "realY: "<<realY << " wantedY: "<<wantedY<<" diff: "<<diff<< "\n";
			//log::out <<"angY: "<<rigidBody->getAngularVelocity().y << "\n";
			//log::out << "velRoty: " << velRotY << " divDiff: "<<(diff / info->timeStep) << "\n";

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
					flatVelDiff /= info->timeStep;
					plr->getRigidBody()->applyWorldForceAtCenterOfMass(ToRp3dVec3(flatVelDiff));
				}
			}

			// -- Extra down force
			if (!flight) {
				rp3d::Vector3 extraDownForce = { 0,0,0 };
				float mass = plr->getRigidBody()->getMass();
				extraDownForce.y += -1 * 9.82 * mass;
				if (plr->getRigidBody()->getLinearVelocity().y < 0) {
					rp3d::Vector3 force = { 0,0,0 };
					extraDownForce.y += -1.2 * 9.82 * mass;
				}
				plr->getRigidBody()->applyLocalForceAtCenterOfMass(extraDownForce);
			}
			plr->getWorld()->unlockPhysics();
		}
	}
}