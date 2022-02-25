#include "gonpch.h"

#include "Player.h"

#include "Engone/Handlers/ObjectHandler.h"

#include "Engone/EventHandler.h"
#include "Keybindings.h"

#include "GameStateEnum.h"

#include "GLFW/glfw3.h"

Player::Player(float x,float y,float z) : GameObject("Player",x,y,z) {
	
	SetModel(engone::GetAsset<engone::ModelAsset>("Player/Player"));
}
void Player::Update(float delta) {
	Movement(delta);
}
glm::vec3 Player::Movement(float delta) {
	using namespace engone;
	Camera* camera = GetCamera();
	if (camera == nullptr)
		return glm::vec3(0);

	glm::vec3 move = glm::vec3(0, 0, 0);

	if (IsKeyPressed(GLFW_KEY_F)) {
		flight = !flight;
	}
	if (IsKeyPressed(GLFW_KEY_H)) {
		thirdPerson = !thirdPerson;
	}

	if (physics.velocity.y>lastVelocity) {
		onGround = true;
	}
	lastVelocity = physics.velocity.y;

	float speed = walkSpeed;
	if (flight) {
		speed = flySpeed;
		if (IsKeybindingDown(KeySprint))
			speed = flyFastSpeed;

		if (IsKeybindingDown(KeyJump)) {
			move.y += speed;
		}
		if (IsKeybindingDown(KeyCrouch)) {
			move.y -= speed;
		}
		physics.gravity = 0;
		physics.velocity = { 0,0,0 };
	} else {
		physics.gravity = -9.81;
		if (IsKeybindingDown(KeySprint))
			speed = sprintSpeed;

		if (IsKeybindingDown(KeyJump)) {
			if (onGround) {
				physics.velocity.y = 5;
				onGround = false;
			}
		}
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

	// Rough way of calculation look vector and movement
	glm::vec3 nmove(move.z * glm::sin(camera->rotation.y) + move.x * glm::sin(camera->rotation.y + glm::half_pi<float>()), move.y,
		move.z * glm::cos(camera->rotation.y) + move.x * glm::cos(camera->rotation.y + glm::half_pi<float>()));

	physics.position += nmove * delta;
	//camera->position += physics.velocity*delta;

	return nmove;
	/*
	if (GetDimension() != nullptr) {
		float terHeight = GetDimension()->TerHeightAtPlayer();
		if (terHeight > collision.sideY(false)) {
			onGround = true;
			position.y = terHeight - collision.y + (collision.h / 2);
			nmove.y = 0;
		}
	}

	std::vector<GameObject*> colL = collision.IsCollidingL();
	for (GameObject* o : colL) {
		MetaStrip* ms = o->metaData.GetMeta(Velocity, Pos, None);
		float velocity[3]{ 0,0,0 };
		if (ms != nullptr) {
			for (int j = 0; j < 3; j++) {
				velocity[j] = ms->floats[j];
				//std::cout << velocity[j] << std::endl;
			}
		}

		// TODO: If nmove[0]==0 and nmove[2]==0 and velocity[0]==0 and move[2]==0 then only check y axel. Saves on performance

		// Calculation Potential Colliding sides
		bool top[]{ nmove.x < 0,nmove.y < 0,nmove.z < 0 };

		bool istop[]{ o->collision.center(0) < collision.center(0),
					o->collision.center(1) < collision.center(1),
					o->collision.center(2) < collision.center(2) };
		for (int i = 0; i < 3; i++) {
			if (istop[i]) {
				if (velocity[i] > 0 && nmove[i] > 0 || velocity[i] < 0 && nmove[i]>0) {
					top[i] = 1;
				} else if (nmove[i] == 0 && velocity > 0) {
					top[i] = 1;
				}
			} else {
				if (velocity[i] < 0 && nmove[0] < 0 || velocity[i]>0 && nmove[i] < 0) {
					top[i] = 0;
				} else if (nmove[i] == 0 && velocity < 0) {
					top[i] = 0;
				}
			}
		}
		//std::cout << top[0] << " " << top[1] << " " << top[2] << std::endl;

		float travelX = (o->collision.sideX(top[0]) - collision.sideX(!top[0])) / (nmove.x + velocity[0]);
		float travelY = (o->collision.sideY(top[1]) - collision.sideY(!top[1])) / (nmove.y + velocity[1]);
		float travelZ = (o->collision.sideZ(top[2]) - collision.sideZ(!top[2])) / (nmove.z + velocity[2]);

		//if(o->GetName() == "red")
		//	std::cout << travelX << " " << travelY << " " << travelZ << std::endl;;

		int axis = -1;
		if (abs(travelX) < abs(travelY) && abs(travelX) < abs(travelZ))
			axis = 0;
		else if (abs(travelY) < abs(travelX) && abs(travelY) < abs(travelZ))
			axis = 1;
		else if (abs(travelZ) < abs(travelX) && abs(travelZ) < abs(travelY))
			axis = 2;

		if (axis == 0) {
			position.x = o->collision.sideX(top[0]) - collision.x + (collision.w / 2)*(top[0] ? 1 : -1);
			*velX = velocity[0];
		} else if (axis == 1) {
			position.y = o->collision.sideY(top[1]) - collision.y + (collision.h / 2)*(top[1] ? 1 : -1);
			*velX = velocity[0];
			*velY = velocity[1];
			*velZ = velocity[2];
			onGround = true;
		} else if (axis == 2) {
			position.z = o->collision.sideZ(top[2]) - collision.z + (collision.d / 2)*(top[2] ? 1 : -1);
			*velZ = velocity[2];
		}
	}*/
}