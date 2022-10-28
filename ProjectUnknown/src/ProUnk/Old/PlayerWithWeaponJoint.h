// Player class but joints are commented and not deleted.
// 2022-10-25

#pragma once

#include "Engone/Engone.h"
#include "Engone/GameObject.h"

#include "ProUnk/Combat/CombatData.h"

namespace prounk {

	class Player : public engone::GameObject {
	public:
		Player(engone::GameGround* ground);

		void update(engone::UpdateInfo& info) override;
		void WeaponUpdate(engone::UpdateInfo& info);
		void Movement(engone::UpdateInfo& info);
		void Input(engone::UpdateInfo& info);

		// Movement and camera
		float zoom = 3;
		float zoomSpeed = 0;

		CombatData combatData;

		engone::GameObject* inventorySword = nullptr;
		struct WeaponState {
			// before weapon was locked
			//float mass = 0;
			bool gravity = false;
			// now
			static const int sampleCount = 3;
			int sampleIndex = 0;
			glm::vec3 lastVelocities[sampleCount];
			glm::vec3 lastPosition;
			double lastTime = 0;
			void reset() {
				for (int i = 0; i < sampleCount; i++) {
					lastVelocities[i] = {};
				}
				lastPosition = {};
				lastTime = 0;
				sampleIndex = 0;
			}
			void sample(engone::GameObject* weapon) {
				using namespace engone;
				if (!weapon)return;

				double now = GetSystemTime();
				double time = now - lastTime;
				if (time == 0) return; // nothing should have happend in 0 seconds.

				glm::vec3 pos = ToGlmVec3(weapon->rigidBody->getTransform().getPosition());
				if (lastTime != 0) { // skip first time
					glm::vec3 diff = pos - lastPosition;
					//log::out << (diff / (float)time) << " " << (1 / (float)time) << "\n";
					lastVelocities[sampleIndex] = (pos - lastPosition) / (float)time; // casting time to float is fine because it will be a small number
					sampleIndex++;
					if (sampleIndex > sampleCount)
						sampleIndex = 0;
				}
				lastPosition = pos;
				lastTime = now;
			}
			glm::vec3 sampledVelocity() {
				glm::vec3 out{};
				for (int i = 0; i < sampleCount; i++) {
					out += lastVelocities[i];
				}
				out /= (float)sampleCount;
				return out;
			}
		};
		//WeaponState weaponState;
		engone::GameObject* heldWeapon = nullptr;

		//rp3d::FixedJoint* weaponJoint = nullptr;
		//engone::GameGround* ground = nullptr; // used when making and destroying joints in update method.

		void setWeapon(engone::GameObject* weapon);

		bool noclip = false;
		bool flight = false;
		void setFlight(bool yes);
		void setNoClip(bool yes);

		float flySpeed = 8.f;
		float flyFastSpeed = 90.f;
		float walkSpeed = 8.f;
		float sprintSpeed = 20.f;
		float jumpForce = 5.f;

		engone::Camera testCam;

		bool onGround = false;

	private:
		float animBlending = 0;
		float animSpeed = 1.7f;
	};
}


#include "ProUnk/Objects/Player.h"

#include "Engone/EventModule.h"
#include "ProUnk/Keybindings.h"
#include "Engone/Window.h"

#include "GLFW/glfw3.h"

#include "Engone/Utilities/rp3d.h"

namespace prounk {
	Player::Player(engone::GameGround* ground) : GameObject() {
		rp3d::Transform t;
		rigidBody = ground->m_pWorld->createRigidBody(t);
		rigidBody->setAngularLockAxisFactor({ 0,1,0 }); // only allow spin (y rotation)
		rigidBody->setIsAllowedToSleep(false);

		setFlight(true);

		//engone::Assets* assets = engone::GetActiveWindow()->getAssets();
		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		//modelAsset = assets->set<engone::ModelAsset>("PlayerBody/PlayerBody"); 
		modelAsset = assets->load<engone::ModelAsset>("Player/Player");
		//modelAsset = assets->set<engone::ModelAsset>("Player/Player");
		animator.asset = modelAsset;

		this->ground = ground;

		loadColliders();
	}
	void Player::update(engone::UpdateInfo& info) {
		animator.update(info.timeStep);
		Input(info);
		Movement(info);
		WeaponUpdate(info);
	}
	void Player::WeaponUpdate(engone::UpdateInfo& info) {
		using namespace engone;

		if (!heldWeapon) return;

		auto transforms = modelAsset->getParentTransforms(nullptr);

		rp3d::Transform swordTrans;

		glm::mat4 modelMatrix = ToMatrix(rigidBody->getTransform());

		AssetInstance* inst = nullptr;
		ArmatureAsset* arm = nullptr;
		glm::mat4 instMat;
		for (int i = 0; i < modelAsset->instances.size(); i++) {
			auto& instance = modelAsset->instances[i];
			if (instance.asset->type == ArmatureAsset::TYPE) {
				inst = &instance;
				arm = (ArmatureAsset*)inst->asset;
				instMat = transforms[i] * inst->localMat;
			}
		}
		if (inst != nullptr) {
			std::vector<glm::mat4> baseBoneMats;
			std::vector<glm::mat4> boneMats = modelAsset->getArmatureTransforms(&animator, instMat, inst, arm, &baseBoneMats);
			Bone& lastBone = arm->bones.back();

			glm::mat4 gripMat = baseBoneMats.back();
			swordTrans = ToTransform(modelMatrix * gripMat);
		}

		heldWeapon->rigidBody->setTransform(swordTrans);
		//heldWeapon->rigidBody->enableGravity(rigidBody->isGravityEnabled());
		//if (!weaponJoint) {
		heldWeapon->rigidBody->setAngularVelocity(rigidBody->getAngularVelocity());
		heldWeapon->rigidBody->setLinearVelocity(rigidBody->getLinearVelocity());
		heldWeapon->rigidBody->resetForce();
		heldWeapon->rigidBody->resetTorque();
		//}

		//weaponState.sample(heldWeapon);
	}
	void Player::setFlight(bool yes) {
		flight = yes;
		rigidBody->enableGravity(!yes);
		if (!yes)
			rigidBody->setLinearDamping(0.f);
		else
			rigidBody->setLinearDamping(7.f);
	}
	void Player::setNoClip(bool yes) {
		noclip = yes;
		setOnlyTrigger(yes);
	}
	void Player::setWeapon(engone::GameObject* weapon) {
		using namespace engone;
		if (heldWeapon) {
			heldWeapon->setOnlyTrigger(false);
			//if (weaponJoint)
			//	ground->m_pWorld->destroyJoint(weaponJoint);
			//heldWeapon->rigidBody->setMass(weaponState.mass);
			//heldWeapon->rigidBody->enableGravity(weaponState.gravity);

			//heldWeapon->rigidBody->setLinearVelocity(ToRp3dVec3(weaponState.sampledVelocity()));

			//weaponState.reset();

			if (heldWeapon->rigidBody->getNbColliders() != 0)
				heldWeapon->rigidBody->getCollider(0)->setUserData(nullptr);

			//weaponJoint = nullptr;
			heldWeapon = nullptr;
		}
		if (!weapon) return; // we just wanted to get rid of the weapon.
		//weaponState.reset();

		// destroy joint to previous weapon.
		heldWeapon = weapon;
		// collider should exist for now
		if (heldWeapon->rigidBody->getNbColliders() != 0) {
			combatData.owner = this;
			heldWeapon->rigidBody->getCollider(0)->setUserData(&combatData);
		}
		//weaponState.mass = weapon->rigidBody->getMass();
		//weaponState.gravity = weapon->rigidBody->isGravityEnabled();

		std::vector<glm::mat4> transforms = modelAsset->getParentTransforms(nullptr);

		rp3d::Vector3 anchor;
		rp3d::Transform swordTrans;

		glm::mat4 modelMatrix = ToMatrix(rigidBody->getTransform());

		AssetInstance* inst = nullptr;
		ArmatureAsset* arm = nullptr;
		glm::mat4 instMat;
		for (int i = 0; i < modelAsset->instances.size(); i++) {
			auto& instance = modelAsset->instances[i];
			if (instance.asset->type == ArmatureAsset::TYPE) {
				inst = &instance;
				arm = (ArmatureAsset*)inst->asset;
				instMat = transforms[i] * inst->localMat;
			}
		}
		if (inst != nullptr) {
			std::vector<glm::mat4> baseBoneMats;
			std::vector<glm::mat4> boneMats = modelAsset->getArmatureTransforms(&animator, instMat, inst, arm, &baseBoneMats);
			Bone& lastBone = arm->bones.back();

			glm::mat4 gripMat = baseBoneMats.back();
			anchor = ToRp3dVec3(gripMat[3]);
			swordTrans = ToTransform(modelMatrix * gripMat);
		}

		rp3d::Vector3 baseVec;
		weapon->setOnlyTrigger(true);
		//heldWeapon->rigidBody->setAngularVelocity(rigidBody->getAngularVelocity());
		//heldWeapon->rigidBody->setLinearVelocity(rigidBody->getLinearVelocity());
		//weapon->rigidBody->resetForce();
		//weapon->rigidBody->resetTorque();
		//weapon->rigidBody->setMass(0.00001f); // set minimal mass so the weapon doesn't affect the characters movement.
		//weapon->rigidBody->setTransform(swordTrans);

		// Joints doesn't work because of animations
		//rp3d::FixedJointInfo fixedInfo(rigidBody, weapon->rigidBody, anchor,baseVec);
		//fixedInfo.isCollisionEnabled = false;
		//weaponJoint = (rp3d::FixedJoint*)engone->m_pWorld->(fixedInfo);
	}
	void Player::Input(engone::UpdateInfo& info) {
		using namespace engone;
		if (IsKeyPressed(GLFW_KEY_G)) {
			setFlight(!flight);
		}
		if (IsKeyPressed(GLFW_KEY_C)) {
			setNoClip(!noclip);
		}
		if (IsKeyPressed(GLFW_KEY_Q)) { // drop/pickup weapon
			if (heldWeapon)
				setWeapon(nullptr);
			else
				setWeapon(inventorySword);
		}
		combatData.update(info);
		if (IsKeyDown(GLFW_MOUSE_BUTTON_LEFT)) { // SLASH_
			if (!combatData.attacking) {
				combatData.skillType = SKILL_SLASH;
				combatData.attack();
				animator.enable("Player", "PlayerStab", { false,1,1,0 });
				AnimatorProperty* prop = animator.getProp("Player");
				if (prop) {
					combatData.animationTime = prop->getRemainingSeconds();
				}
			}
		}
	}
	void Player::Movement(engone::UpdateInfo& info) {
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
		//log::out << "speed: " << zoomSpeed << " "<<zoom<<"\n";

		glm::vec3 flatMove{};
		if (glm::length(flatInput) != 0) {
			flatMove = speed * glm::normalize(camera->getFlatLookVector() * flatInput.z + camera->getRightVector() * flatInput.x);
		}
		glm::vec3 moveDir = flatMove;
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

		if (rigidBody) {
			rp3d::Vector3 rot = ToEuler(rigidBody->getTransform().getOrientation());
			float realY = rot.y;
			float wantY = realY;

			if (IsKeybindingDown(KeyMoveCamera) || zoom == 0) {
				wantY = camera->getRotation().y - glm::pi<float>();
			} else if (glm::length(flatMove) != 0) {
				wantY = std::atan2(flatMove.x, flatMove.z);
			}

			float dv = AngleDifference(wantY, realY) / (info.timeStep);
			dv *= 0.3; // snap speed
			float velRotY = dv - rigidBody->getAngularVelocity().y;
			// inertia tensor needs to be accounted for when applying torque.

			//rp3d::Vector3 newVel = rigidBody->getAngularVelocity();
			//newVel.y += velRotY;
			rp3d::Vector3 newVel = { 0,dv,0 };
			rigidBody->setAngularVelocity(newVel);

			//log::out << "realY: "<<realY << " wantedY: "<<wantedY<<" diff: "<<diff<< "\n";
			//log::out <<"angY: "<<rigidBody->getAngularVelocity().y << "\n";
			//log::out << "velRoty: " << velRotY << " divDiff: "<<(diff / info.timeStep) << "\n";

			glm::vec3 bodyVel = ToGlmVec3(rigidBody->getLinearVelocity());
			float keepY = bodyVel.y;
			float moveDirY = moveDir.y;
			if (!flight) {
				moveDir.y = 0;
				bodyVel.y = 0;
			}
			glm::vec3 flatVelDiff = moveDir - bodyVel;
			bool dontMove = false;
			if (!rigidBody->isGravityEnabled() && glm::length(moveDir) == 0) {
				dontMove = true;
			}
			if (!dontMove) {
				float tolerance = 0.0001;
				if (glm::length(flatVelDiff) < tolerance && !flight) {
					glm::vec3 other{};
					if (!flight)
						other.y = keepY + moveDirY;
					rigidBody->setLinearVelocity(ToRp3dVec3(other));
				} else {
					flatVelDiff *= 0.25;
					if (!flight)
						flatVelDiff.y = moveDirY;
					flatVelDiff /= info.timeStep;
					rigidBody->applyWorldForceAtCenterOfMass(ToRp3dVec3(flatVelDiff));
				}
			}
		}
	}
}