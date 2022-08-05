
#include "Player.h"

#include "Engone/EventModule.h"
#include "Keybindings.h"
#include "Engone/Window.h"

#include "GLFW/glfw3.h"

#include "Engone/Utilities/rp3d.h"

Player::Player(engone::Engone* engone) : GameObject(engone) {
	this->engone = engone;
	rp3d::Transform t;
	rigidBody = engone->m_pWorld->createRigidBody(t);
	rigidBody->setAngularLockAxisFactor({ 0,1,0 }); // only allow spin (y rotation)
	rigidBody->enableGravity(false);
	rigidBody->setIsAllowedToSleep(false);
	rigidBody->setLinearDamping(10.f);

	engone::Assets* assets = engone::GetActiveWindow()->getAssets();
	modelAsset = assets->set<engone::ModelAsset>("PlayerBody/PlayerBody"); 

	loadColliders(engone);
}
void Player::update(engone::UpdateInfo& info) {
	Input(info);
	Movement(info);
}
void Player::setWeapon(engone::GameObject* weapon) {
	using namespace engone;
	if (heldWeapon) {
		heldWeapon->setOnlyTrigger(false);
		engone->m_pWorld->destroyJoint(weaponJoint);
		weaponJoint = nullptr;
		heldWeapon = nullptr;
	}
	if (!weapon) return; // we just wanted to get rid of the weapon.

	// destroy joint to previous weapon.
	heldWeapon = weapon;

	auto transforms = modelAsset->getParentTransforms(nullptr);

	rp3d::Transform swordTrans;
	rp3d::Vector3 anchor;
	log::out << rigidBody->getTransform().getOrientation()<<"\n";
	glm::mat4 modelMatrix = ToMatrix(rigidBody->getTransform());
	glm::quat rot;
	DecomposeGlm(modelMatrix, nullptr, &rot, nullptr);
	glm::mat4 sec = glm::mat4_cast(rot);
	DecomposeGlm(sec, nullptr, &rot, nullptr);
	sec = glm::mat4_cast(rot);
	DecomposeGlm(sec, nullptr, &rot, nullptr);
	sec = glm::mat4_cast(rot);
	DecomposeGlm(sec, nullptr, &rot, nullptr);
	//DecomposeGlm(sec, nullptr, &rot, nullptr);
	log::out << rot<<"\n";
	for (int i = 0; i < modelAsset->instances.size(); i++) {
		auto& inst = modelAsset->instances[i];
		if (inst.name == "PlayerGrip") {
			glm::mat4 loc = transforms[i] * inst.localMat;
			anchor = ToRp3dVec3(loc[3]);
			swordTrans = ToTransform(modelMatrix*loc);
			break;
		}
	}
	weapon->setOnlyTrigger(true);
	rp3d::Vector3 baseVec;
	weapon->rigidBody->setAngularVelocity(baseVec);
	weapon->rigidBody->setLinearVelocity(baseVec);
	weapon->rigidBody->resetForce();
	weapon->rigidBody->resetTorque();
	//log::out << engone::ToEuler(swordTrans.getOrientation()) << "\n";
	//log::out << swordTrans.getOrientation() << "\n";
	//swordTrans.setPosition(swordTrans.getPosition()+)
	weapon->rigidBody->setTransform(swordTrans);
	rp3d::FixedJointInfo fixedInfo(rigidBody, weapon->rigidBody, anchor,baseVec);
	weaponJoint = engone->m_pWorld->createJoint(fixedInfo);
}
void Player::Input(engone::UpdateInfo& info) {
	using namespace engone;
	if (IsKeyPressed(GLFW_KEY_G)) {
		rigidBody->enableGravity(!rigidBody->isGravityEnabled());
		if (rigidBody->isGravityEnabled()) {
			rigidBody->setLinearDamping(0.f);
		} else {
			rigidBody->setLinearDamping(10.f);
		}
	}
	// This is supposed to disable collision and still allow movement.
	// For the bit change to work, you need to step away from other colliders.
	if (IsKeyPressed(GLFW_KEY_C)) {
		noclip = !noclip;
		setOnlyTrigger(noclip);
		//if (rigidBody->getNbColliders() != 0) {
		//	//rp3d::Transform bye;
		//	//bye.setPosition({ -9999, -9999, -9999 });
		//	//rp3d::Transform last = rigidBody->getTransform();
		//	//rigidBody->setTransform(bye);
		//	rp3d::Collider* col = rigidBody->getCollider(0);
		//	uint16_t bits = col->getCollideWithMaskBits();
		//	bits = bits ^ 1;
		//	col->setCollideWithMaskBits(bits);
		//	//rigidBody->setTransform(last);
		//}
	}
	if (IsKeyPressed(GLFW_KEY_Q)) {
		if (heldWeapon)
			setWeapon(nullptr);
		else
			setWeapon(inventorySword);
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
	if (IsKeybindingDown(KeySprint))
		speed = sprintSpeed;

	glm::vec3 inputDir = { 0,0,0 };
	if (IsKeybindingDown(KeyForward)) {
		inputDir.z += 1;
	}
	if (IsKeybindingDown(KeyLeft)) {
		inputDir.x -= 1;
	}
	if (IsKeybindingDown(KeyBack)) {
		inputDir.z -= 1;
	}
	if (IsKeybindingDown(KeyRight)) {
		inputDir.x += 1;
	}
	float scroll = IsScrolledY();
	if (scroll) {
		zoomOut -= scroll;
		if (zoomOut < 0) zoomOut = 0;
	}

	glm::vec3 moveDir{};
	if (glm::length(inputDir) != 0) {
		moveDir = speed * glm::normalize(camera->getFlatLookVector() * inputDir.z + camera->getRightVector() * inputDir.x);
	}
	if (IsKeybindingDown(KeyJump)) {
		moveDir.y += jumpForce;
	}

	if (rigidBody) {
		rp3d::Vector3 rot = ToEuler(rigidBody->getTransform().getOrientation());
		float ry = rot.y;
		float diff = AngleDifference(camera->getRotation().y, ry);
		float velRotY = diff / info.timeStep - rigidBody->getAngularVelocity().y;
		rp3d::Vector3 torque = { 0,velRotY / info.timeStep,0 };
		//torque *= 0.5; // if you want to weaken it.
		//log::out << "\n";
		//log::out << "ry: "<<ry << "\n";
		//log::out << "diff: " << diff <<" camY: "<< camera->getRotation().y<<" ang: "<<rigidBody->getAngularVelocity().y << "\n";
		//log::out << "velRoty: " << velRotY << " divDiff: "<<(diff / info.timeStep) << "\n";
		//log::out << torque << "\n";
		//log::out << rigidBody->getTransform().getOrientation()<<"\n";
		// this will sometimes crash the game if z and x axis aren't locked. quaternion will become 0 for some reason.
		// the reason is because the torque is increased.
		// to fix this there is a limit to how much torque.

		if(torque.length()<10000)
			rigidBody->applyLocalTorque(torque);

		glm::vec3 bodyVel = ToGlmVec3(rigidBody->getLinearVelocity());
		float keepY = bodyVel.y;
		float moveDirY = moveDir.y;
		moveDir.y = 0;
		bodyVel.y = 0;

		glm::vec3 flatVelDiff = moveDir - bodyVel;
		//log::out << moveDir << " "<<bodyVel << " "<<flatVelDiff<<"\n";
		float tolerance = 0.0001;
		if (glm::length(flatVelDiff) < tolerance) {
			glm::vec3 other = { 0,keepY + moveDirY,0 };
			rigidBody->setLinearVelocity(ToRp3dVec3(other));
		} else {
			flatVelDiff *= 0.25; // weaken acceleration
			flatVelDiff.y = moveDirY;
			flatVelDiff /= info.timeStep;
			rigidBody->applyWorldForceAtCenterOfMass(ToRp3dVec3(flatVelDiff));
		}
	}
}