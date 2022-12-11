#include "ProUnk/PlayerController.h"

#include "Engone/EventModule.h"
#include "ProUnk/Keybindings.h"
#include "Engone/Window.h"

//#include "GLFW/glfw3.h"

//#include "Engone/Utilities/rp3d.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/Combat/Combat.h"

namespace prounk {
	PlayerController::PlayerController() {
		
	}
	void PlayerController::setPlayerObject(engone::EngineObject* player) {
		m_player = player;

		setFlight(true);
	}
	void PlayerController::update(engone::LoopInfo& info) {
		//animator.update(info.timeStep);
		Input(info);
		Movement(info);
		WeaponUpdate(info);
	}
	void PlayerController::WeaponUpdate(engone::LoopInfo& info) {
		using namespace engone;

		if (!heldWeapon) return;

		auto transforms = m_player->modelAsset->getParentTransforms(nullptr);

		rp3d::Transform swordTrans;

		glm::mat4 modelMatrix = ToMatrix(m_player->rigidBody->getTransform());

		AssetInstance* inst = nullptr;
		ArmatureAsset* arm = nullptr;
		glm::mat4 instMat;
		for (int i = 0; i < m_player->modelAsset->instances.size(); i++) {
			auto& instance = m_player->modelAsset->instances[i];
			if (instance.asset->type == ArmatureAsset::TYPE) {
				inst = &instance;
				arm = (ArmatureAsset*)inst->asset;
				instMat = transforms[i] * inst->localMat;
			}
		}
		if (inst != nullptr) {
			std::vector<glm::mat4> baseBoneMats;
			std::vector<glm::mat4> boneMats = m_player->modelAsset->getArmatureTransforms(&m_player->animator, instMat, inst, arm, &baseBoneMats);
			Bone& lastBone = arm->bones.back();

			glm::mat4 gripMat = baseBoneMats.back();
			swordTrans = ToTransform(modelMatrix * gripMat);
		}

		heldWeapon->rigidBody->setTransform(swordTrans);

		heldWeapon->rigidBody->setAngularVelocity(m_player->rigidBody->getAngularVelocity());
		heldWeapon->rigidBody->setLinearVelocity(m_player->rigidBody->getLinearVelocity());
		heldWeapon->rigidBody->resetForce();
		heldWeapon->rigidBody->resetTorque();

		//heldWeapon->rigidBody->enableGravity(rigidBody->isGravityEnabled());
		//weaponState.sample(heldWeapon);
	}
	void PlayerController::setFlight(bool yes) {
		flight = yes;
		m_player->rigidBody->enableGravity(!yes);
		if (!yes)
			m_player->rigidBody->setLinearDamping(0.f);
		else
			m_player->rigidBody->setLinearDamping(7.f);
	}
	void PlayerController::setNoClip(bool yes) {
		noclip = yes;
		m_player->setOnlyTrigger(yes);
	}
	void PlayerController::setWeapon(engone::LoopInfo& info, engone::EngineObject* weapon) {
		using namespace engone;
		if (heldWeapon) {
			heldWeapon->setOnlyTrigger(false);
			m_world->netEditObject(heldWeapon->getUUID(), 0, false);

			//heldWeapon->rigidBody->enableGravity(weaponState.gravity);
			//heldWeapon->rigidBody->setLinearVelocity(ToRp3dVec3(weaponState.sampledVelocity()));
			//weaponState.reset();

			SetCombatData(m_player, heldWeapon, false);
			m_world->netEditCombatData(heldWeapon->getUUID(), m_player->getUUID(), false);
			//if (heldWeapon->rigidBody->getNbColliders() != 0) {
			//	heldWeapon->rigidBody->getCollider(0)->setUserData(0);
			//	heldWeapon->flags &= ~OBJECT_HAS_COMBATDATA;
			//	heldWeapon->userData = nullptr;
			//}
			//	//ground->netEditObject(heldWeapon->getUUID(), 1, 0, );

			heldWeapon = nullptr;
		}
		if (!weapon) return; // we just wanted to get rid of the weapon.
		//weaponState.reset();

		// destroy joint to previous weapon.
		heldWeapon = weapon;
		// collider should exist for now
		SetCombatData(m_player, heldWeapon, true);
		m_world->netEditCombatData(heldWeapon->getUUID(), m_player->getUUID(), true);

		//if (heldWeapon->rigidBody->getNbColliders() != 0) {
		//	heldWeapon->rigidBody->getCollider(0)->setUserData((void*)COLLIDER_IS_DAMAGE);
		//	heldWeapon->flags |= OBJECT_HAS_COMBATDATA;
		//	heldWeapon->userData = userData;
		//	//ground->netEditObject(heldWeapon->getUUID(), 1, 0, getUUID().data[0], getUUID().data[1], 0);
		//	
		//}
		//weaponState.gravity = weapon->rigidBody->isGravityEnabled();

		std::vector<glm::mat4> transforms = m_player->modelAsset->getParentTransforms(nullptr);

		rp3d::Vector3 anchor;
		rp3d::Transform swordTrans;

		glm::mat4 modelMatrix = ToMatrix(m_player->rigidBody->getTransform());

		AssetInstance* inst = nullptr;
		ArmatureAsset* arm = nullptr;
		glm::mat4 instMat;
		for (int i = 0; i < m_player->modelAsset->instances.size(); i++) {
			auto& instance = m_player->modelAsset->instances[i];
			if (instance.asset->type == ArmatureAsset::TYPE) {
				inst = &instance;
				arm = (ArmatureAsset*)inst->asset;
				instMat = transforms[i] * inst->localMat;
			}
		}
		if (inst != nullptr) {
			std::vector<glm::mat4> baseBoneMats;
			std::vector<glm::mat4> boneMats = m_player->modelAsset->getArmatureTransforms(&m_player->animator, instMat, inst, arm, &baseBoneMats);
			Bone& lastBone = arm->bones.back();

			glm::mat4 gripMat = baseBoneMats.back();
			anchor = ToRp3dVec3(gripMat[3]);
			swordTrans = ToTransform(modelMatrix * gripMat);
		}

		rp3d::Vector3 baseVec;
		weapon->setOnlyTrigger(true);
		m_world->netEditObject(weapon->getUUID(), 0, true);

		//heldWeapon->rigidBody->setAngularVelocity(rigidBody->getAngularVelocity());
		//heldWeapon->rigidBody->setLinearVelocity(rigidBody->getLinearVelocity());
		//weapon->rigidBody->resetForce();
		//weapon->rigidBody->resetTorque();
		//weapon->rigidBody->setTransform(swordTrans);
	}
	void PlayerController::Input(engone::LoopInfo& info) {
		using namespace engone;
		if (IsKeyPressed(GLFW_KEY_G)) {
			setFlight(!flight);
		}
		if (IsKeyPressed(GLFW_KEY_C)) {
			setNoClip(!noclip);
		}
		if (IsKeyPressed(GLFW_KEY_Q)) { // drop/pickup weapon
			if (!isDead()) {
				if (heldWeapon)
					setWeapon(info, nullptr);
				else
					setWeapon(info, inventorySword);
			}
		}
		CombatData* combatData = nullptr;
		if (m_player->userData) {
			EntityRegistry::Entry& entry = m_world->entityRegistry.getEntry(m_player->userData);
			combatData = entry.combatData;
		}
		if (IsKeyPressed(GLFW_KEY_R)) {
			if (isDead()) {
				deathTime = 0;
				setDead(false);
				m_player->rigidBody->setAngularLockAxisFactor({ 0,1,0 });
				//auto tr = rigidBody->getTransform(); // only reset orientation
				//tr.setOrientation({ 0,0,0,1.f });
				//rigidBody->setTransform(tr);
				m_player->rigidBody->setTransform({}); // reset position too

				combatData->health = combatData->getMaxHealth();
			}
		}
		if (IsKeyPressed(GLFW_KEY_K)) {
			combatData->health = 0;
		}
		if (IsKeyDown(GLFW_MOUSE_BUTTON_LEFT)) {
			if (!combatData->attacking) {
				combatData->skillType = SKILL_SLASH;
				combatData->attack();
				m_player->animator.enable("Player", "PlayerDownSwing", { false,1,1,0 });
				m_world->netAnimateObject(m_player->getUUID(), "Player", "PlayerDownSwing", false, 1, 1, 0);
				AnimatorProperty* prop = m_player->animator.getProp("Player");
				if (prop) {
					combatData->animationTime = prop->getRemainingSeconds();
				}
			}
		}
		if (IsKeyDown(GLFW_KEY_F)) {
			if (!combatData->attacking) {
				combatData->skillType = SKILL_SIDE_SLASH;
				combatData->attack();
				m_player->animator.enable("Player", "PlayerSideSwing", { false,1,1,0 });
				m_world->netAnimateObject(m_player->getUUID(), "Player", "PlayerSideSwing", false, 1, 1, 0);
				AnimatorProperty* prop = m_player->animator.getProp("Player");
				if (prop) {
					combatData->animationTime = prop->getRemainingSeconds();
				}
			}
		}
	}
	void PlayerController::Movement(engone::LoopInfo& info) {
		using namespace engone;

		Window* window = engone::GetActiveWindow();
		if (!window) return;
		Renderer* renderer = window->getRenderer(); // if window is valid, render and camera should as well. BUT i may change somethings which would break this assumption. So i'll keep it.
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
		if (!isDead()) {
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

		// death logic
		CombatData* combatData = nullptr;
		if (m_player->userData) {
			EntityRegistry::Entry& entry = m_world->entityRegistry.getEntry(m_player->userData);
			combatData = entry.combatData;
		}
		deathTime -= info.timeStep;
		//printf("%f\n", deathTime);
		if (deathTime < 0) {
			deathTime = 0;
			if (isDead()) {
				setDead(false);
				m_player->rigidBody->setAngularLockAxisFactor({ 0,1,0 }); // only allow spin (y rotation)
				m_player->setTransform({ 0,0,0 });
				m_player->rigidBody->setLinearVelocity({ 0,0,0 });
				m_player->rigidBody->setAngularVelocity({ 0,0,0 });
				combatData->health = combatData->getMaxHealth();
			}
		}
		if (!isDead() && combatData->health == 0) {
			deathTime = 5;

			setDead(true);
			zoom = 4;

			setFlight(false);
			if (heldWeapon)
				setWeapon(info, nullptr);

			m_player->rigidBody->setAngularLockAxisFactor({ 1,1,1 }); // only allow spin (y rotation)

			float angleStrength = 200.f * deathShockStrength;
			rp3d::Vector3 angRand = { (float)GetRandom() - 0.5f,(float)GetRandom() - 0.5f,(float)GetRandom() - 0.5f };
			angRand *= angleStrength;
			m_player->rigidBody->applyLocalTorque(angRand);

			float velStrength = 200.0f * deathShockStrength;
			rp3d::Vector3 velRand = { (float)GetRandom() - 0.5f,(float)GetRandom() / 2.f,(float)GetRandom() - 0.5f };
			velRand *= velStrength;
			//log::out << angRand << " " << velRand << "\n";
			m_player->rigidBody->applyWorldForceAtCenterOfMass(velRand);
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
		if (m_player->rigidBody && !isDead()) {
			rp3d::Vector3 rot = ToEuler(m_player->rigidBody->getTransform().getOrientation());
			float realY = rot.y;
			float wantY = realY;

			if (IsKeybindingDown(KeyMoveCamera) || zoom == 0) {
				wantY = camera->getRotation().y - glm::pi<float>();
			} else if (glm::length(flatMove) != 0) {
				wantY = std::atan2(flatMove.x, flatMove.z);
			}

			float dv = AngleDifference(wantY, realY) / (info.timeStep);
			dv *= 0.3; // snap speed
			float velRotY = dv - m_player->rigidBody->getAngularVelocity().y;
			// inertia tensor needs to be accounted for when applying torque.

			//rp3d::Vector3 newVel = rigidBody->getAngularVelocity();
			//newVel.y += velRotY;
			rp3d::Vector3 newVel = { 0,dv,0 };
			m_player->rigidBody->setAngularVelocity(newVel);

			//log::out << "realY: "<<realY << " wantedY: "<<wantedY<<" diff: "<<diff<< "\n";
			//log::out <<"angY: "<<rigidBody->getAngularVelocity().y << "\n";
			//log::out << "velRoty: " << velRotY << " divDiff: "<<(diff / info.timeStep) << "\n";

			glm::vec3 bodyVel = ToGlmVec3(m_player->rigidBody->getLinearVelocity());
			float keepY = bodyVel.y;
			float moveDirY = moveDir.y;
			if (!flight) {
				moveDir.y = 0;
				bodyVel.y = 0;
			}
			glm::vec3 flatVelDiff = moveDir - bodyVel;
			bool dontMove = false;
			if (!m_player->rigidBody->isGravityEnabled() && glm::length(moveDir) == 0) {
				dontMove = true;
			}
			if (!dontMove) {
				float tolerance = 0.0001;
				if (glm::length(flatVelDiff) < tolerance && !flight) {
					glm::vec3 other{};
					if (!flight)
						other.y = keepY + moveDirY;
					m_player->rigidBody->setLinearVelocity(ToRp3dVec3(other));
				} else {
					flatVelDiff *= 0.25;
					if (!flight)
						flatVelDiff.y = moveDirY;
					flatVelDiff /= info.timeStep;
					m_player->rigidBody->applyWorldForceAtCenterOfMass(ToRp3dVec3(flatVelDiff));
				}
			}
			if (!flight) {
				rp3d::Vector3 extraDownForce = { 0,0,0 };
				float mass = m_player->rigidBody->getMass();
				extraDownForce.y += -0.34 * 9.82 * mass;
				if (m_player->rigidBody->getLinearVelocity().y < 0) {
					rp3d::Vector3 force = { 0,0,0 };
					extraDownForce.y += -1.2 * 9.82 * mass;
				}
				m_player->rigidBody->applyLocalForceAtCenterOfMass(extraDownForce);
			}
		}
	}
}