#include "Gamecore.h"

#include "Objects/MagicStaff.h"

//#include "Visual.h"
#include "Utility/Keyboard.h"

#include <ctime>
#include <chrono>

#include "Objects/Cube.h"
#include "Objects/NetPlayer.h"

#include "Chat.h"
#include "Utility/GameOptions.h"

void Gamecore::LoadGameData() {// ALL game assets should be added here(Not interface stuff). Remember to load texture first and then mesh
	
	AddTexture("magicstaff_staff", "assets/textures/magicstaff_staff");
	AddTexture("magicstaff_fireball", "assets/textures/magicstaff_fireball");
	AddAnimation("magicstaff_idle", "assets/animations/MagicStaffIdle");
	AddMesh("magicstaff_staff", "assets/models/staff");
	AddMesh("magicstaff_fireball", "assets/models/fireball");
	
	AddAnimation("player_idle", "assets/animations/PlayerIdle");
	AddMesh("player_body", "assets/models/Body");
	AddMesh("player_head", "assets/models/Head");
	AddMesh("player_ruarm", "assets/models/RUArm");
	AddMesh("player_rlarm", "assets/models/RLArm");

	AddTexture("noise", "assets/textures/noise");
	AddMesh("Parkour", "assets/models/Parkour");
	
}
Gamecore::Gamecore(){
	RenderInit(1600,900);

	ReadOptions();
	/*float persistance = GetOptionf("persistance");
	float lacunarity = GetOptionf("lacunarity");
	int octaves = GetOptioni("octaves");
	float scale = GetOptionf("scale");
	std::cout << scale << " " << persistance << " "<<lacunarity << " "<<octaves<<" " << std::endl;
	*/
	InitManager();
	InitInterface();
	InitEditor();
	//InitVisual();
	LoadGameData();
	UpdateKeyboard(false);
	//InitChat(&updateObjects);

	SetPauseMode(true);
	
	GetPlayer()->SetPosition(0, 0, 0);

	GetPlayer()->CreateHitbox();
	AddUpdateO(GetPlayer());

	MetaStrip pMeta(Velocity,Pos);
	pMeta.bools[0] = true;
	GetPlayer()->metaData.AddMeta(pMeta);

	//GetPlayer()->collision.objects = &updateObjects;
	/*
	GameObject* staff = new MagicStaff(0, 2, 0);
	AddRenderO("tex",staff);
	AddUpdateO(staff);*/
	
	/*
	Cube* cube = new Cube(0,0,0,10,2,10,0.4,0.8,0.9,1);
	cube->SetName("Flat");
	AddRenderO("color", cube);
	AddUpdateO(cube);
	*/
	
	Cube* cube2 = new Cube(4, 0, -3, 8, 8, 2, 0.4, 1, 1, 1);
	cube2->SetName("Wall1");
	cube2->renderMesh = false;
	AddRenderO("color", cube2);
	AddUpdateO(cube2);
	/*
	Cube* cube3 = new Cube(4.95 + 4, 10, 5, 8, 8, 2, 0.4, 1, 1, 1);
	cube3->renderMesh = false;
	cube3->SetName("Wall2");
	AddRenderO("color", cube3);
	AddUpdateO(cube3);
	*/
	//cube2->SetRotation(0,0.5,0);

	//cube2->SetRotation(0, 0, 0);
	/*
	MetaStrip cubeV(Velocity,Pos,None);
	cubeV.floats[2] = -0.5;
	cubeV.floats[1] = -0.5;
	cube3->metaData.AddMeta(cubeV);
	*/
	
	//GameObject* noice = new GameObject(0,2,0,"Parkour");
	//AddRenderO("tex", noice);
	
	//AddUpdateO(cube);
	/*
	Cube* cube2 = new Cube(1, 0.75, -2, 1, 0.5, 2, 0.9, 0.2, 0.2, 1);
	AddRenderO("color", cube2);
	AddUpdateO(cube2);
	cube2->SetName("red");
	MetaStrip meta(Polation, Pos, Y);
	meta.floats[0] = 0.75;
	meta.floats[1] = 5;
	meta.floats[2] = 0.03;
	cube2->metaData.AddMeta(meta);
	MetaStrip metax(Polation, Pos, X);
	metax.floats[0] = 2;
	metax.floats[1] = 6;
	metax.floats[2] = (metax.floats[1]-metax.floats[0])/((meta.floats[1]-meta.floats[0])/meta.floats[2]);
	cube2->metaData.AddMeta(metax);

	Cube* cube3 = new Cube(2, 7, 1, .5, .5, .5, 0.2, 0.9, 0.2, 1);
	AddRenderO("color", cube3);
	AddUpdateO(cube3);
	
	MetaStrip meta3(Polation, Pos, X);
	meta3.floats[0] = -6;
	meta3.floats[1] = 6;
	meta3.floats[2] = 0.1;
	cube3->metaData.AddMeta(meta3);
	
	Cube* cube4 = new Cube(-4, 1.5, -2, 2, 2, 0.5, 0.9, 0.2, 0.2, 1);
	AddRenderO("color", cube4);
	AddUpdateO(cube4);
	*/
	/*
	GameObject* netp = new NetPlayer(0, 2, 2);
	AddRenderO("color", netp);
	AddUpdateO(netp);
	*/
	/*
	MetaStrip netrot(Velocity,Rot);
	netrot.floats[1] = 0.01f;
	//netp->metaData.AddMeta(netrot);
	cube4->metaData.AddMeta(netrot);
	
	Light* light = new Light(1,7,2,20,1,1,1,0.85,0.01,0.005);
	AddLight(light);
	*/
	GetPlayer()->doMove = true;
	GetPlayer()->Update(0);
	UpdateProj();
	
	/*
	Dimension overworld();
	Biome plain(0,0.7,0,4);
	plain.ColorRange(0,0.7,0.5,1,0,0.7);
	Biome mount(0.7, 1, 4, 30);
	mount.ColorRange(0.2,0.6,0.2,0.4,0.2,0.3);
	overworld.AddBiome(plain);
	overworld.AddBiome(mount);
	overworld.Init(&player->position);

	AddDimension(overworld);
	*/
	/*
	GLuint vbuf;
	glGenVertexArrays(1, &vbuf);
	glBindVertexArray(vbuf);
	GLuint buf;
	glGenBuffers(1, &buf);
	float box[]{
		0,0.8,0,
		-0.8,-0.8,0,
		0.8,-0.8,0
	};
	glBindBuffer(GL_ARRAY_BUFFER, buf);
	glBufferData(GL_ARRAY_BUFFER,sizeof(box),box,GL_STATIC_DRAW);
	glEnableVertexArrayAttrib(vbuf, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,0, (const void*)(0 * sizeof(float)));
	*/
	/*
	float fl[]{
		0,-10,0,
		1,-10,0.5,
		1,-11,0,
		0,-11,0.5,
	};
	unsigned int in[]{
		0,1,
		2,1,
		2,0
	};
	BufferContainer buf;
	buf.type = 1;
	buf.Setup(false,fl,3*4,in,2*3);
	buf.SetAttrib(0,3,3,0);*/
	//buf.SetAttrib(1,2,5,3);
	// Loop
	int lastTime=GetTime();
	int lastDTime = lastTime;
	int FPS = 0;
	int tickSpeed = 0;
	float delta;
	while (RenderRunning()) {
		int now = GetTime();
		if (now-lastTime>1000) {
			lastTime = now;
			SetWindowTitle(("Reigai Dimension - "+std::to_string(FPS)+" fps").c_str());
			FPS = 0;
		}

		delta = (now - lastDTime)/1000.f;
		lastDTime = now;
		tickSpeed++;
		if (tickSpeed==10) {
			tickSpeed = 0;
			FPS++;
			if (HasFocus()&&!GetPauseMode()) {
				Update(delta);
			}
		}
		//cube2->metaData.GetMeta(Velocity, Pos, None)->floats[1] -= 0.1/60;
		// Update
		//std::cout << "okay2214t" << std::endl;

		//cube2->SetRotation(GetCamera()->rotation);

		glClearColor(0.4f, 0.9f, 1.f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Render();
		/*
		SwitchBlendDepth(false);
		BindShader("tex");
		BindTexture("blank");
		ObjectTransform(glm::mat4(1));
		//ObjectColor(0, 0, 0, 0);

		glLineWidth(2.f);
		buf.Draw();*/
		

		/*
		Location tempo;
		tempo.Translate(glm::vec3(0,2,0));
		tempo.Rotate(cube3->rotation);
		tempo.Translate(glm::vec3(0,2,5));
		cube3->rotation.y += 0.04;
		//cube3->rotation.x += 0.005;
		cube3->position = tempo.vec();
		light->position = tempo.vec();
		*/

		//DrawVisual();

		glfwSwapBuffers(GetWindow());
		glfwPollEvents();
	}
	// remove buffer and stuff
	RenderTermin();
}
void Gamecore::AddUpdateO(GameObject* o) {
	updateObjects.push_back(o);
	//o->collision.objects = &updateObjects;
}
void Gamecore::DelUpdateO(GameObject* o) {
	for (int i = 0; i < updateObjects.size(); i++) {
		if (updateObjects[i] == o) {
			updateObjects.erase(updateObjects.begin() + i);
			break;
		}
	}
}
int Gamecore::GetTime() {
	return std::chrono::system_clock::now().time_since_epoch().count()/10000;
}
void pri(std::string s) {
	std::cout << s << std::endl;
}
int tim = 0;
void Gamecore::Update(float delta) {
	// Object updates
	GetPlayer()->doMove = !GetCursorMode();// && !GetChatMode();
	//UpdateProj();

	for (GameObject* o0 : updateObjects) {
		o0->velocity = glm::vec3(0, 0, 0);
	}
	// Player stuff
	GetPlayer()->velocity += GetPlayer()->Movement(delta);

	// Sort slowest velocity first
	for (int i = 0; i < updateObjects.size();i++) {
		GameObject* o0 = updateObjects[i];
		MetaStrip* ms = o0->metaData.GetMeta(Velocity, Pos, None);

		//o0->velocity = glm::vec3(0, 0, 0);
		if(ms!=nullptr){
			o0->velocity += glm::vec3(ms->floats[0],ms->floats[1],ms->floats[2]);
		}

		// Collision detection. Change velocity
		if (o0->isSolid) {
			bool hit=false;
			glm::vec3 finalV = o0->velocity;
			for (int j = i+1; j < updateObjects.size(); j++) { // WILL BREAK IF SOMETHING HAS BEEN HIT. KEEP THIS?
				GameObject* o1 = updateObjects[j];
				if (o1->isSolid && o0->collision.IsClose(&o1->collision)) {
					// check for sphere type or plane type?

					glm::vec3 v = o0->WillCollide(o1, delta);
					//o0->position += v * delta;
					if (v!=o0->velocity) {
						// Has collided... do stuff
						
						hit = true;
						finalV = v;
						// player hit ground?

						/*
						MetaStrip* ms = o0->metaData.GetMeta(Velocity, Pos, None);
						if (ms != nullptr) {
							ms->floats[0] = 0;
							ms->floats[1] = 0;
							ms->floats[2] = 0;
						}*/
						break;
					}
				}
			}
			o0->position += finalV * delta;
		}

		//o0->MetaUpdate(delta);
		o0->Update(delta);

		/*
		if (o0->isSolid) {
			Collision* c0 = &o0->collision;//GetHitbox(o->standard_hitbox);
			
			// Test Expensive collision for close enough objects.
			ClientPlayer* cl = dynamic_cast<ClientPlayer*>(o0);
			for (GameObject* o1 : updateObjects) {
				if (o1->isSolid&&o0!=o1) {
					Collision* c1 = &o1->collision;
					if (c0->IsClose(c1)) { // Close enough
						if (c0->type == 1 || c1->type == 1) {
							// COLLISION HAS HAPPENED
							std::cout << "Circle Collision not Implemented" << std::endl;
							//break;
						} else {
							glm::vec3 v;
							if (o0->weight > o1->weight) {
								v = c0->collision(c1);// Expensive detection
							} else {
								v = c1->collision(c0);// Expensive detection
							}
							if (v.x != 0 || v.y != 0 || v.z != 0) {
								if (cl!=nullptr) {
									if (v.y!=0) {
										cl->onGround = true;
									} else {
										cl->onGround = false;
									}
								}
								// The object with smaller weight should be pushed
								if (o0->weight < o1->weight) {
									MetaStrip* mVel = o0->metaData.GetMeta(Velocity, Pos, None);
									if (mVel != nullptr) {
										
										//glm::vec3 oVel(mVel->floats[0], mVel->floats[1], mVel->floats[2]);
										//glm::vec3 nVel = oVel + glm::normalize(v)*glm::length(oVel);
										//mVel->floats[0] = nVel.x;
										//mVel->floats[1] = nVel.y;
										//mVel->floats[2] = nVel.z;
										mVel->floats[0] = 0;
										mVel->floats[1] = 0;
										mVel->floats[2] = 0;
									}
									o0->position += v;
								} else {
									MetaStrip* mVel = o1->metaData.GetMeta(Velocity, Pos, None);
									if (mVel != nullptr) {
										//glm::vec3 oVel(mVel->floats[0], mVel->floats[1], mVel->floats[2]);
										//glm::vec3 nVel = oVel + glm::normalize(v)*glm::length(oVel);
										//mVel->floats[0] = nVel.x;
										//mVel->floats[1] = nVel.y;
										//mVel->floats[2] = nVel.z;
										mVel->floats[0] = 0;
										mVel->floats[1] = 0;
										mVel->floats[2] = 0;
									}
									o1->position -= v;
								}
								//break;
							}
						}
					}
				}
			}
		}*/
	}
	if (!GetPlayer()->freeCam) {
		GetCamera()->position = GetPlayer()->position;
	} else if (GetPlayer()->freeCam&&GetPlayer()->moveCam) {
		
	} else {

	}
	UpdateProj();
	
	if(GetDimension()!=nullptr)
		GetDimension()->CleanChunks();

	UpdateInterface();
	//glm::vec3 v = GetCamera()->rotation;
	//std::cout << v.x << " " << v.y << " " << v.z << std::endl;

	// Update dimension
	
}
void Gamecore::Render() {
	if (GetMenu()==Gameplay) {
		RenderGame();
	}
	RenderInterface();
}