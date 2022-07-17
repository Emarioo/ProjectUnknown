
#include "Player.h"

#include "Engone/EventModule.h"
#include "Keybindings.h"
#include "Engone/Window.h"

#include "GLFW/glfw3.h"

Player::Player(engone::Engone* engone) : GameObject(engone) {
	
	rp3d::Transform t;
	rigidBody = engone->m_pWorld->createRigidBody(t);
	rigidBody->setAngularLockAxisFactor({ 0,1,0 }); // only allow spin (y rotation)
	//rigidBody->setLinearLockAxisFactor({ 1,0,1 });
	rigidBody->enableGravity(false);
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
				col->getMaterial().setFrictionCoefficient(3.f);
				col->getMaterial().setBounciness(0.5f);
			}
		}
	}
}
void Player::update(engone::UpdateInfo& info) {
	Movement(info.timeStep);
}

void Player::Movement(float delta) {
	using namespace engone;
	Camera* camera = engone::GetActiveWindow()->getRenderer()->getCamera();
	if (!camera)
		return;

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

	float speed = 20;
	if (rigidBody->isGravityEnabled()) {
		speed = 150; // Needs some more power when doing applyForce
	}
	if (IsKeybindingDown(KeySprint))
		speed *= 5;

	if (IsKeybindingDown(KeyJump)) {
		move.y += speed;
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
	float scroll = IsScrolledY();
	if (scroll) {
		zoomOut -= scroll;
		if (zoomOut < 0) zoomOut = 0;
	}

	rp3d::Vector3 vect = { move.z * glm::sin(camera->rotation.y) + move.x * glm::sin(camera->rotation.y + glm::half_pi<float>()), move.y,
		move.z * glm::cos(camera->rotation.y) + move.x * glm::cos(camera->rotation.y + glm::half_pi<float>()) };

	float length = vect.lengthSquare();

	if (rigidBody) {
		rp3d::Transform tr = rigidBody->getTransform();
		tr.setOrientation(rp3d::Quaternion::fromEulerAngles({0,camera->rotation.y,0}));
		rigidBody->setTransform(tr);
		
		if (length != 0) {
			if (!rigidBody->isGravityEnabled()) {
				rigidBody->setLinearVelocity(vect);
			} else {
				rp3d::Vector3 moveDir = rigidBody->getLinearVelocity();
				vect.y = moveDir.y;
				rp3d::Vector3 lookDir = vect;
				vect -= moveDir;
				float cosTheta = lookDir.dot(moveDir)/moveDir.length()/lookDir.length();

				float lookVelocity = cosTheta * moveDir.length();

				if (lookVelocity < 20) {

					// Math to change how much force i can apply.
					rigidBody->applyWorldForceAtCenterOfMass(vect);
				}
			}
		}
	}
}