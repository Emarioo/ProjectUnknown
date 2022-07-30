
#include "Player.h"

#include "Engone/EventModule.h"
#include "Keybindings.h"
#include "Engone/Window.h"

#include "GLFW/glfw3.h"

#include "Engone/Utilities/rp3d.h"

Player::Player(engone::Engone* engone) : GameObject(engone) {
	
	rp3d::Transform t;
	rigidBody = engone->m_pWorld->createRigidBody(t);
	rigidBody->setAngularLockAxisFactor({ 0,1,0 }); // only allow spin (y rotation)
	//rigidBody->setLinearLockAxisFactor({ 1,0,1 });
	rigidBody->enableGravity(false);
	rigidBody->setIsAllowedToSleep(false);
	rigidBody->setLinearDamping(10.f);
	engone::Assets* assets = engone::GetActiveWindow()->getAssets();
	modelAsset = assets->set<engone::ModelAsset>("PlayerBody/PlayerBody");
	//rigidBody->setMass(10);
	for (auto& inst : modelAsset->instances) {
		if (inst.asset->type == engone::ColliderAsset::TYPE) {
			engone::ColliderAsset* asset = inst.asset->cast<engone::ColliderAsset>();
			if (asset->colliderType == engone::ColliderAsset::Type::Cube) {
				glm::vec3 scale2 = asset->cube.size;
				
				rp3d::Vector3 scale = *(rp3d::Vector3*)&asset->cube.size;
				rp3d::BoxShape* box = engone->m_pCommon->createBoxShape(scale);
				
				rp3d::Transform tr;
				tr.setFromOpenGL((float*)&inst.localMat);
				rigidBody->addCollider(box, tr);
				auto col = rigidBody->getCollider(rigidBody->getNbColliders() - 1);
				col->getMaterial().setFrictionCoefficient(0.7f);
				col->getMaterial().setBounciness(0.03f);
			}
		}
	}
}
void Player::update(engone::UpdateInfo& info) {
	Movement(info);
}

void Player::Movement(engone::UpdateInfo& info) {
	using namespace engone;

	Window* window = engone::GetActiveWindow();
	if (!window) return;
	Renderer* renderer = window->getRenderer(); // if window is valid, render and camera should as well. BUT i may change somethings which would break this assumption. So i'll keep it.
	if (!renderer) return;
	Camera* camera = renderer->getCamera();
	if (!camera) return;

	glm::vec3 move = glm::vec3(0, 0, 0);

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
		if (rigidBody->getNbColliders() != 0) {
			
			rp3d::Collider* col = rigidBody->getCollider(0);
			uint16_t bits = col->getCollideWithMaskBits();
			bits = bits^1;
			col->setCollideWithMaskBits(bits);
		}
	}

	float speed = 10;
	if (rigidBody->isGravityEnabled()) {
		speed = 150; // Needs some more power when doing applyForce
	}
	if (IsKeybindingDown(KeySprint))
		speed *= 3;

	if(rigidBody->isGravityEnabled()){
		if (IsKeybindingPressed(KeyJump)) {
			move.y += speed*3;
		}
	} else {
		if (IsKeybindingDown(KeyJump)) {
			move.y += speed;
		}
	}
	if (IsKeybindingDown(KeyCrouch)) {
		move.y -= speed;
	}
	if (IsKeybindingDown(KeyForward)) {
		move.z -= speed;
	}
	if (IsKeybindingDown(KeyBack)) {
		move.z += speed;
	}
	if (IsKeybindingDown(KeyRight)) {
		move.x += speed;
	}
	if (IsKeybindingDown(KeyLeft)) {
		move.x -= speed;
	}
	if (IsKeyDown('u')) {
		move.y += 10;
	}
	float scroll = IsScrolledY();
	if (scroll) {
		//auto col = rigidBody->getCollider(rigidBody->getNbColliders() - 1);
		////col->getMaterial().setFrictionCoefficient(3.f);
		//float newBounce = col->getMaterial().getBounciness() + scroll / 100.f;
		//if (newBounce > 1) newBounce = 1;
		//if (newBounce < 0) newBounce = 0;
		//col->getMaterial().setBounciness(newBounce);
		zoomOut -= scroll;
		if (zoomOut < 0) zoomOut = 0;
	}

	rp3d::Vector3 vect = { move.z * glm::sin(camera->getRotation().y) + move.x * glm::sin(camera->getRotation().y + glm::half_pi<float>()), move.y,
		move.z * glm::cos(camera->getRotation().y) + move.x * glm::cos(camera->getRotation().y + glm::half_pi<float>()) };

	float length = vect.lengthSquare();
	//rigidBody->applyWorldForceAtCenterOfMass({0,10,0});
	if (rigidBody) {
		rp3d::Transform tr = rigidBody->getTransform();
		//rigidBody->setTransform(tr);

		rp3d::Vector3 rot = ToEuler(rigidBody->getTransform().getOrientation());
		float ry = rot.y;
		float diff = AngleDifference(camera->getRotation().y,ry);
		float velRotY = diff / info.timeStep-rigidBody->getAngularVelocity().y;
		rp3d::Vector3 torque = { 0,velRotY/info.timeStep,0 };
		//torque *= 0.1; // if you want to weaken it.
		rigidBody->applyLocalTorque(torque);
		//log::out << camera->rotation.y << " " << rot.y << "\n";
		
		// take in existing torque? kind of only relevant if other forces was applied current update.

		if (length != 0) {
			if (!rigidBody->isGravityEnabled()) {
				rigidBody->setLinearVelocity(vect);
			} else {
				auto col = rigidBody->getCollider(0);
				
				rp3d::Vector3 moveDir = rigidBody->getLinearVelocity();
				//log::out << "norm " << collisionNormal << "\n";
				if (collisionNormal != glm::vec3(0, 0, 0)) {

					// N F

					//Friction = Normal*u
					//glm::vec3 force = {0,0,0};
					//glm::vec3 N = collisionNormal;
					//glm::vec3 W = collisionNormal;
					//glm::vec3 N = collisionNormal;

					//float diffY = (1-collisionNormal.y)*10*info.timeStep - moveDir.y;
					//float force = diffY / info.timeStep;
					//log::out << "force " << force << "\n";
					
					//glm::vec3 N0 = {0,1,0};
					//glm::vec3 W = *(glm::vec3*)&vect;
					//float length = glm::length(W);
					//W = glm::normalize(W);
					//glm::vec3 N = -glm::normalize(collisionNormal);
					//glm::vec3 D = W;
					////log::out << "N " << N << "\n";
					//glm::vec3 F = (D + N)*length;
					//vect = *(rp3d::Vector3*)&F;
					//glm::vec3 norm = glm::normalize(collisionNormal);
					//vect.y += 30;
					//vect += *(rp3d::Vector3*)&norm;
				}

				// still can't climb hills

				//log::out << "vect " << *(glm::vec3*)&vect<< "\n";
				collisionNormal = { 0,0,0 };

				moveDir.y = 0;
				if (moveDir.length() > 25) { // Max speed here
					vect += -moveDir / info.timeStep * 0.4;
				}
				rigidBody->applyWorldForceAtCenterOfMass(vect);
			}
		} else {
			// when not moving
			rp3d::Vector3 moveDir = rigidBody->getLinearVelocity();
			moveDir.y = 0;
			moveDir = -moveDir/info.timeStep*0.05;
			rigidBody->applyWorldForceAtCenterOfMass(moveDir);
		}
	}
	if(rigidBody)
		lastForce = *(glm::vec3*)&rigidBody->getForce();
}