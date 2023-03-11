#include "GameCode/PlayerController.h"

#include "Engone/Window.h"

#include "ProUnk/Objects/BasicObjects.h"
#include "ProUnk/Keybindings.h"

static const float flySpeed = 8.f;
static const float flyFastSpeed = 90.f;
static const float walkSpeed = 8.f;
static const float sprintSpeed = 15.f;
static const float jumpForce = 17.f;
static const float deathShockStrength = 1.f; // how much the player "jumps" when they die.
static const float deathItemShockStrength = 1.f; // how much items fly on death.
		
// Todo: These should not be static
static bool flight = false;
static float zoom = 3;
static float zoomSpeed = 0;
static float deathTime = 0;
static float jumpTime = 0;
static float jumpDelay = 0.4;
static bool followTargetRotation=false;
static float targetRotation=0;

void PlayerSetFlight(engone::EngineObject* plr, bool yes){
    Assert(plr);

    flight = yes;
    plr->getWorld()->lockPhysics();
    
    plr->getRigidBody()->setIsSleeping(false);
    plr->getRigidBody()->enableGravity(!yes);
    plr->getRigidBody()->setLinearDamping(yes? 7.0 : 0.f);

    plr->getWorld()->unlockPhysics();
}
void PlayerDropInventory(engone::EngineObject* plr) {
    using namespace engone;
    using namespace prounk;
    Assert(plr);
    // EngineObject* plr = requestPlayer();
    // if (!plr) return;
    prounk::Inventory* inv = prounk::GetInventory(plr);
    prounk::Dimension* dim = (prounk::Dimension*)plr->getWorld()->getUserData();
    
    auto creatureInfo = dim->getParent()->objectInfoRegistry.getCreatureInfo(plr->getObjectInfo());
    engone::EngineObject* heldObject = dim->getWorld()->getObject(creatureInfo->heldObject);

    // Drops the held object at a specific position
    // Dimension* dim = dims[0]; // Todo: the dimension should be the one the player is in.
    // auto heldObject = requestHeldObject();
    for (int i = 0; i < inv->getSlotSize();i++) {
        prounk::Item* item = inv->getItem(i);

        if (item->getType() == 0)
            continue;

        auto object = CreateItem(dim, *item);
        *item = prounk::Item(); // clear slot

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

        dim->getParent()->netAddGeneral(object);
        dim->getParent()->netMoveObject(object);

        dim->getWorld()->releaseAccess(object->getUUID());
    }
    // releaseHeldObject(heldObject);
    // releasePlayer(plr);
}
// extern "C" {
//    void extern __declspec(dllexport) SupDude(void* info, void* plr){
//         printf("pointers %p, %p\n",info,plr);
//    }
// }
#define Key_Forward GLFW_KEY_W
#define Key_Left GLFW_KEY_A
#define Key_Back GLFW_KEY_S
#define Key_Right GLFW_KEY_D
#define Key_Jump GLFW_KEY_SPACE
#define Key_Crouch GLFW_KEY_LEFT_CONTROL
#define Key_Sprint GLFW_KEY_LEFT_SHIFT
#define Key_MoveCamera GLFW_MOUSE_BUTTON_RIGHT
#define Key_Pause GLFW_KEY_ESCAPE
    
void ENGONE_EXPORT UpdatePlayerMovement(void* _info, void* _plr) {
	using namespace engone;
	using namespace prounk;
	
	engone::LoopInfo& info = *(LoopInfo*)_info;
	engone::EngineObject* plr = (EngineObject*)_plr;

	Window* window = info.window;
	if (!window) return;
	CommonRenderer* renderer = info.window->getCommonRenderer();
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
		// log::out << "Scroll "<<scroll<<" "<<zoomSpeed<<"\n";
		if (scroll) {
			if (zoom == minZoom)
				zoom = 0;
			zoomSpeed -= scroll * (0.8 + zoom * 0.3);
		}
	}

	// death logic
	prounk::CombatData* combatData = prounk::GetCombatData(plr);
	if (combatData->getHealth() == 0 && deathTime == 0) {
		//log::out << "Kill player\n";
		// kill player
		deathTime = 5; // respawn time

		PlayerSetFlight(plr,false);
		info.window->enableFirstPerson(false);

		plr->getWorld()->lockPhysics();
		// make player jump
		plr->getRigidBody()->setAngularLockAxisFactor({ 1,1,1 });

		rp3d::Vector3 randomTorque = { (float)GetRandom() - 0.5f,(float)GetRandom() - 0.5f,(float)GetRandom() - 0.5f };
		randomTorque *= deathShockStrength * 0.1f; // don't want to spin so much
		//randomTorque /= info.timeStep * info.timeStep;
		plr->getRigidBody()->applyLocalTorque(randomTorque);

		rp3d::Vector3 randomForce = { (float)GetRandom() - 0.5f,(float)GetRandom() / 2.f,(float)GetRandom() - 0.5f };
		randomForce *= deathShockStrength;
		//randomForce /= info.timeStep * info.timeStep;
		plr->getRigidBody()->applyWorldForceAtCenterOfMass(randomForce);
		plr->getWorld()->unlockPhysics();

		PlayerDropInventory(plr);
		// if (!keepInventory)
			// dropAllItems();
	}

	deathTime -= info.timeStep;
	if (deathTime < 0) {
		deathTime = 0;
		if (prounk::IsDead(plr)) {
			//log::out << "Respawn player\n";
			info.window->enableFirstPerson(true);
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
		CommonRenderer* renderer =  info.window->getCommonRenderer();
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
	jumpTime -= info.timeStep;
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

		float dv = AngleDifference(wantY, realY) / (info.timeStep);
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

		//-- Extra down force
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
	// releasePlayer(plr);
}