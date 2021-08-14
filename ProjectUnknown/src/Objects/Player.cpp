#include "propch.h"

#include "Player.h"

#include "Engone/Handlers/ObjectHandler.h"

#include "Keybindings.h"

#include "GameStateEnum.h"

#include "GLFW/glfw3.h"

Player::Player(float x,float y,float z) : GameObject("Player",x,y,z) {
	weight = 1;
	/*
	renderComponent.SetModel("Player");
	collisionComponent.SetCollider(renderComponent.model->colliderName);
	renderComponent.animator.Enable("goblin_idle", { 0,true,1,1 });
	renderComponent.animator.Enable("goblin_run", { 0,true,0,1 });
	*/
}
void Player::Update(float delta) {
	if (engine::IsKeyActionDown(KeyForward)) {
		if (animBlending < 1 && animSpeed>0)
			animBlending += animSpeed * delta;
	} else {
		if (animBlending > 0)
			animBlending -= animSpeed * delta;
	}
	if (engine::IsKeyActionDown(KeyCrouch)) {
		renderComponent.animator.Speed("goblin_run", 
			flight|| !engine::CheckState(GameState::CameraToPlayer) ? camFastSpeed/camSpeed : sprintSpeed/walkSpeed
		);
	} else {
		renderComponent.animator.Speed("goblin_run", 1);
	}

	renderComponent.animator.Blend("goblin_idle", animBlending);
	renderComponent.animator.Blend("goblin_run", 1 - animBlending);

	renderComponent.animator.Update(delta);

	Movement(delta);
}
glm::vec3 Player::Movement(float delta) {
	Camera* camera = engine::GetCamera();
	if (camera == nullptr)
		return glm::vec3(0);
	
	glm::vec3 move = glm::vec3(0, 0, 0);
	
	if (true) { // Disable movement - useless?
		if (engine::IsKey(GLFW_KEY_N)) {
			if (!freeCamT) {
				engine::SetState(GameState::CameraToPlayer, !engine::CheckState(GameState::CameraToPlayer));
			}
			freeCamT = true;
		} else {
			freeCamT = false;
		}
		if (engine::IsKey(GLFW_KEY_F)) {
			if (!flightT) {
				flight =! flight;
			}
			flightT = true;
		} else {
			flightT = false;
		}
		if (engine::IsKey(GLFW_KEY_H)) {
			if (!thirdPersonT) {
				thirdPerson = !thirdPerson;
			}
			thirdPersonT = true;
		} else {
			thirdPersonT = false;
		}

		float speed = walkSpeed;
		if (!engine::CheckState(GameState::CameraToPlayer) ||flight) {
			speed = camSpeed;
			if (engine::IsKeyActionDown(KeySprint))
				speed = camFastSpeed;

			if (engine::IsKeyActionDown(KeyJump)) {
				move.y += speed;
			}
			if (engine::IsKeyActionDown(KeyCrouch)) {
				move.y -= speed;
			}
			if(flight)
				velocity.y = 0;
		} else {
			velocity.y += gravity;
			if (engine::IsKeyActionDown(KeySprint))
				speed = sprintSpeed;

			if (engine::IsKeyActionDown(KeyJump)) {
				if (onGround) {
					velocity.y = 5;
					onGround = false;
				}
			}
		}

		if (engine::IsKeyActionDown(KeyForward)) {
			move.z -= speed;
		}
		if (engine::IsKeyActionDown(KeyBack)) {
			move.z += speed;
		}
		if (engine::IsKeyActionDown(KeyRight)) {
			move.x += speed;
		}
		if (engine::IsKeyActionDown(KeyLeft)) {
			move.x -= speed;
		}
		/*
		if (GetKeyState(VK_CONTROL) < 0) { // TODO: better crouch method
			crouchMode = true;
			//collision.h = 1;
			if (!crouchToggle) {

			}
			crouchToggle = true;
		} else {
			crouchMode = false;
			if (crouchToggle) {
				if (onGround) {
					float t = 0.1*60.f;
					*velY += 0.5 / t - t * gravity;// Crouch bounce
				}
			}
			if (*velY <= gravity * -1) {
				//collision.h = 2;
			}
			crouchToggle = false;
		}
		*/
	}
	// Rough way of calculation look vector and movement
	glm::vec3 nmove(move.z * glm::sin(camera->rotation.y) + move.x * glm::sin(camera->rotation.y + glm::half_pi<float>()), move.y,
		move.z * glm::cos(camera->rotation.y) + move.x * glm::cos(camera->rotation.y + glm::half_pi<float>()));

	// Collision detection TODO: Improve collision detection to only use one loop through and Detect multiple collision at once with help from velocities
	
	velocity = nmove;
	camera->velocity = nmove;

	//camera->position.z += 8*0.016f;
	camera->position += nmove* delta;
	
	if (!engine::CheckState(GameState::CameraToPlayer)) {
			camera->velocity = nmove;
			camera->position += nmove * delta;
	} else {
		velocity = nmove;
		position += nmove * delta;
		
		SetRotation(0, camera->rotation.y + glm::pi<float>(), 0);

		engine::Location camPos; // Camera offset
		camPos.Translate(position + glm::vec3(0, 0.5f, 0));
		if (thirdPerson) {
			camPos.Rotate(camera->rotation);
			camPos.Translate(0.5, 0, 3);
		}
		camera->velocity = velocity;
		camera->position = camPos.vec();
		
	}
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