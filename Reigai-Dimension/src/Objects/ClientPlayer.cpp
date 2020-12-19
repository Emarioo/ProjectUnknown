#include "ClientPlayer.h"

ClientPlayer::ClientPlayer() {
	name = "Player";
	weight = 1;

	//animation.SetData(dManager::GetAnim("player_idle"));

	//bone.SetData(dManager::GetBone("player_bone"));
	//bone.SetAnim(&animation);
	/*
	animation.SetData(dManager::GetAnim(""));
	bone.SetAnim(&animation);
	bone.SetBone(dManager::GetBone(""));
	mesh.AddMesh(dManager::GetMesh("player_body"));
	mesh.SetBone(&bone);
	dManager::AddComponent(&mesh);
	*/
	//collider.SetData(dManager::AddColl("ClientPlayer"));
	//collider.MakeCube(0, -0.5, 0, 1, 3, 1);
}
void ClientPlayer::SetCamera(Camera* c) {
	camera = c;
}
void ClientPlayer::PreComponents() {
	
	/*
	Location body;
	body.Translate(position);
	body.Rotate(rotation);
	pbody.SetMatrix(body.mat());

	Location head(body);
	head.Translate(idle.GetPos("Head"));
	phead.SetMatrix(head.mat());

	Location arm(body);
	arm.Translate(idle.GetPos("RUArm"));
	arm.Rotate(idle.GetRot("RUArm"));
	pruarm.SetMatrix(arm.mat());

	arm.Translate(glm::vec3(0, -0.814433, 0));
	prlarm.SetMatrix(arm.mat());*/
}
std::vector<ColliderComponent*> ClientPlayer::GetColliders() {
	std::vector<ColliderComponent*> out;
	//out.push_back(&collider);
	return out;
}
void ClientPlayer::Update(float delta) {
	animation.Update(delta);
	Location body;
	body.Translate(position);
	body.Rotate(rotation);
	//mesh.matrices[0] = body.mat();
}
bool camToggle = false;
bool moveToggle = false;
bool crouchToggle = false;
glm::vec3 ClientPlayer::Movement(float delta) {
	// TODO: Only needs to change when Player is created or these are destroyed
	/*

	MetaStrip* mVel = metaData.GetMeta(Velocity, Pos, None);
	float temp[3]{ 0,0,0 };
	float* velX = &temp[0];
	float* velY = &temp[1];
	float* velZ = &temp[2];
	if (mVel != nullptr) {
		velX = &mVel->floats[0];
		velY = &mVel->floats[1];
		velZ = &mVel->floats[2];
	}
	*/
	glm::vec3 move = glm::vec3(0, 0, 0);
	if (doMove) {
		if (GetKeyState('N') < 0) {
			if (!camToggle) {
				freeCam = !freeCam;
				if (!freeCam) {
					if(camera!=nullptr)
						SetPosition(camera->position.x,camera->position.y,camera->position.z);
				}
			}
			camToggle = true;
		} else {
			camToggle = false;
		}
		if (freeCam) {
			if (GetKeyState('M') < 0) {
				if (!moveToggle) {
					moveCam = !moveCam;
				}
				moveToggle = true;
			} else {
				moveToggle = false;
			}
		}
		if (true||freeCam&&moveCam) {
			float speed = camSpeed;
			if (GetKeyState(VK_LSHIFT) < 0) {
				speed = camFastSpeed;
			}
			if (GetKeyState(VK_SPACE) < 0) {
				move.y += speed;
			}
			if (GetKeyState(VK_CONTROL) < 0) {
				move.y -= speed;
			}
			if (GetKeyState('W') < 0) {
				move.z -= speed;
			}
			if (GetKeyState('S') < 0) {
				move.z += speed;
			}
			if (GetKeyState('D') < 0) {
				move.x += speed;
			}
			if (GetKeyState('A') < 0) {
				move.x -= speed;
			}
		} else {
			
			// *velY += gravity; // Switch this with an acceleration Meta
			
			float speed = walkSpeed;
			if (GetKeyState('S') < 0) {
				move.z += speed;
			}
			if (GetKeyState('D') < 0) {
				move.x += speed;
			}
			if (GetKeyState('A') < 0) {
				move.x -= speed;
			}
			if (GetKeyState(VK_SHIFT) < 0) {
				sprintMode = true;
				speed = sprintSpeed;
			} else {
				sprintMode = false;
			}
			if (GetKeyState('W') < 0) {
				move.z -= speed;
			}
			if (GetKeyState(VK_SPACE) < 0) {
				if (onGround) {

					onGround = false;
				}
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
	}
	// Rought way of calculation look vector and movement
	glm::vec3 nmove(move.z*glm::sin(rotation.y) + move.x*glm::sin(rotation.y + glm::pi<float>() / 2), move.y,
		move.z*glm::cos(rotation.y) + move.x*glm::cos(rotation.y + glm::pi<float>() / 2));

	/*
	if (!freeCam||!moveCam) {
		nmove.x += *velX;
		nmove.y += *velY;
		nmove.z += *velZ;
	}
	*/

	// Collision detection TODO: Improve collision detection to only use one loop through and Detect multiple collision at once with help from velocities

	//rotation = camera->rotation;
	if(camera!=nullptr)
		SetRotationA(0,camera->rotation.y,0);
	if (!freeCam) {
		//position += nmove * delta;
		return nmove;
	} else if (freeCam&&moveCam) {
		if(camera!=nullptr)
			camera->position += nmove * delta;
	} else {
		//bug::out +bug::AQUA+ nmove + '\n';
		return nmove;
		//position += nmove * delta;
	}
	return glm::vec3(0,0,0);
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