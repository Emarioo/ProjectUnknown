#include "Gamecore.h"

#include "Objects/MagicStaff.h"

#include "Utility/Keyboard.h"

#include <ctime>
#include <chrono>

#include "Objects/Cube.h"
#include "Objects/NetPlayer.h"
#include "Objects/Tutorial.h"
#include "Objects/OTemplate.h"

#include "Chat.h"
#include "Utility/GameOptions.h"

#include "InterfaceManager.h"
#include "ObjectManager.h"

#include "MagicEditor.h"

namespace gamecore {
	int vecLimit = 100;
	int lineLimit = 100;
	BufferContainer hitbox;
	float* hitboxVec;
	unsigned int* hitboxInd;
	void LoadGameData() {// ALL game assets should be added here(Not interface stuff)
		renderer::AddTexture("magicstaff_staff", "assets/textures/magicstaff_staff");
		renderer::AddTexture("magicstaff_fireball", "assets/textures/magicstaff_fireball");
		dManager::AddAnim("idle", "assets/animations/MagicStaffIdle");
		dManager::AddMesh("staff", "assets/meshes/staff");
		dManager::AddMesh("fireball", "assets/meshes/fireball");

		dManager::AddAnim("player_idle", "assets/animations/PlayerIdle");
		dManager::AddMesh("player_body", "assets/meshes/Body");
		dManager::AddMesh("player_head", "assets/meshes/Head");
		dManager::AddMesh("player_ruarm", "assets/meshes/RUArm");
		dManager::AddMesh("player_rlarm", "assets/meshes/RLArm");
		
		renderer::AddTexture("noise", "assets/textures/noise");
		dManager::AddMesh("parkour", "assets/meshes/Parkour");
		dManager::AddColl("parkour", "assets/colliders/Parkour");

		dManager::AddMesh("floor", "assets/meshes/Floor");
		dManager::AddColl("floor", "assets/colliders/Floor");

		dManager::AddMesh("vertexcolor", "assets/meshes/VertexColor");
		dManager::AddColl("vertexcolor", "assets/colliders/VertexColor");
	}
	void Init() {
		renderer::Init();

		ReadOptions();
		/*float persistance = GetOptionf("persistance");
		float lacunarity = GetOptionf("lacunarity");
		int octaves = GetOptioni("octaves");
		float scale = GetOptionf("scale");
		std::cout << scale << " " << persistance << " "<<lacunarity << " "<<octaves<<" " << std::endl;*/

		LoadGameData();

		dManager::Init();
		iManager::Init();
		oManager::Init();
		
		oManager::GetPlayer()->SetPosition(0, 2, 0);

		InitEditor();
		UpdateKeyboard(false);
		//InitChat(&updateObjects);

		iManager::SetPauseMode(true);

		MetaStrip pMeta(Velocity, Pos);
		pMeta.bools[0] = true;
		oManager::GetPlayer()->metaData.AddMeta(pMeta);

		oManager::GetPlayer()->renderHitbox = true;

		oManager::AddObject(new MagicStaff(0, 0, -10));
		Tutorial* tut = new Tutorial(0, 0, 0);
		tut->renderHitbox = true;
		oManager::AddObject(tut);
		
		OTemplate* temp = new OTemplate(0, 5, 0);
		temp->renderHitbox = true;
		oManager::AddObject(temp);

		/*
		Cube* cube = new Cube(0,0,0,10,2,10,0.4,0.8,0.9,1);
		cube->SetName("Flat");
		*/
		/*
		Cube cube2 = Cube(4, 0, -3, 8, 8, 2, 0.4, 1, 1, 1);
		cube2.SetName("Wall1");
		oManager::AddObject(cube2);
		*/
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
		/*
		GameObject noice(0, 0, 0, dManager::GetMesh("parkour"));
		//AddRenderO("tex", noice);
		//AddUpdateO(*noice);
		noice.collision.UseCollider(dManager::GetColl("parkour"));
		dManager::AddObject(noice);
		*/
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

		/*
		MetaStrip netrot(Velocity,Rot);
		netrot.floats[1] = 0.01f;
		//netp->metaData.AddMeta(netrot);
		cube4->metaData.AddMeta(netrot);

		Light* light = new Light(1,7,2,20,1,1,1,0.85,0.01,0.005);
		AddLight(light);
		*/
		oManager::GetPlayer()->doMove = true;
		oManager::GetPlayer()->Update(0.1);
		renderer::UpdateProj();

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
		hitbox.type = 1;
		hitbox.Setup(true, nullptr, 3*vecLimit,nullptr,2* lineLimit);
		hitbox.SetAttrib(0, 3, 3, 0);
		hitboxVec = new float[vecLimit *3];
		hitboxInd = new unsigned int[lineLimit *2];
		
		// Loop
		int lastTime = GetTime();
		int lastDTime = lastTime;
		int FPS = 0;
		int tickSpeed = 0;
		float delta;

		/*
		Option to lock fps
		*/
		while (renderer::RenderRunning()) {
			int now = GetTime();
			if (now - lastTime > 1000) {
				lastTime = now;
				renderer::SetWindowTitle(("Reigai Dimension  " + std::to_string(FPS) + " fps").c_str());
				FPS = 0;
				//std::cout << "Running" << std::endl;
			}

			delta = (now - lastDTime) / 1000.f;
			lastDTime = now;
			tickSpeed++;
			if (tickSpeed == 1) {
				tickSpeed = 0;
				FPS++;
				if (renderer::HasFocus() && !iManager::GetPauseMode()) {
					//Update(1.f/60);//delta);
					Update(delta);
				}
			}
			glClearColor(0.4f, 0.9f, 1.f, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			Render();
			

			glfwSwapBuffers(renderer::GetWindow());
			glfwPollEvents();
		}
		// remove buffer and stuff or not?
		renderer::RenderTermin();
	}
	int GetTime() {
		return std::chrono::system_clock::now().time_since_epoch().count() / 10000;
	}
	int tim = 0;
	void Update(float delta) {
		oManager::GetPlayer()->doMove = !renderer::GetCursorMode();// && !GetChatMode();

		for (int i = 0; i < oManager::GetObjects()->size(); i++) {
			oManager::GetObjects()->at(i)->velocity = glm::vec3(0, 0, 0);
		}
		// Additional Player Movement
		oManager::GetPlayer()->velocity += oManager::GetPlayer()->Movement(delta);

		// Sort slowest velocity first
		for (int i = 0; i < oManager::GetObjects()->size(); i++) {
			GameObject* o0 = oManager::GetObjects()->at(i);
			MetaStrip* ms = o0->metaData.GetMeta(Velocity, Pos, None);

			if (ms != nullptr) {
				o0->velocity += glm::vec3(ms->floats[0], ms->floats[1], ms->floats[2]);
			}
			
			if (o0->GetColliders().size()>0) {
				// If velocity has changed then go trough the list of all objects again
				for (int j = i + 1; j < oManager::GetObjects()->size(); j++) {
					GameObject* o1 = oManager::GetObjects()->at(j);
					if (o0->IsClose(o1)) {

						glm::vec3 v = o0->WillCollide(o1, delta);
						if (v != o0->velocity) {
							o0->velocity = v;
							/* Reset static velocity?
							MetaStrip* ms = o0->metaData.GetMeta(Velocity, Pos, None);
							if (ms != nullptr) {
								ms->floats[0] = 0;
								ms->floats[1] = 0;
								ms->floats[2] = 0;
							}*/
						}
					}
				}
			}
			o0->position += o0->velocity * delta;
			
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
		if (!oManager::GetPlayer()->freeCam) {
			renderer::GetCamera()->position = oManager::GetPlayer()->position;
		} else if (oManager::GetPlayer()->freeCam && oManager::GetPlayer()->moveCam) {

		} else {

		}
		renderer::UpdateProj();

		if (dManager::GetDimension() != nullptr)
			dManager::GetDimension()->CleanChunks();

		iManager::UpdateInterface();
		// Update dimension

		// Update mesh transformations should maybe be moved to render
		for (GameObject* o : *oManager::GetObjects()) {
			if (o->renderMesh) {
				o->PreComponents();
			}
		}
	}
	bool reachedLimit = false;
	void Render() {
		// Run preComponents here?
		renderer::SwitchBlendDepth(false);
		renderer::BindShader(MaterialType::ColorMat);
		for (MeshComponent* m : *dManager::GetMeshComponents()) {
			if (m->mesh != nullptr) {
				if (m->mesh->material == MaterialType::ColorMat) {
					renderer::DrawMesh(m->mesh, m->matrix);
				}
			} else {
				if (!m->hasError) {
					bug::out + bug::RED + "MeshData in MeshComponent is nullptr\n";
					m->hasError = true;
				}
			}
		}
		renderer::BindShader(MaterialType::TextureMat);
		for (MeshComponent* m : *dManager::GetMeshComponents()) {
			if (m->mesh != nullptr) {
				if (m->mesh->material == MaterialType::TextureMat) {
					renderer::DrawMesh(m->mesh, m->matrix);
				}
			}
		}
		renderer::BindShader(MaterialType::OutlineMat);
		renderer::ObjectTransform(glm::mat4(1));
		renderer::ObjectColor(1, 1, 1, 1);
		glLineWidth(2.f);
		for (GameObject* o : *oManager::GetObjects()) {// Draws a hitbox for every object by thowing all the lines from colliders into a vertex buffer
			if (o->renderHitbox) {
				int atVec = 0;
				int atInd = 0;
				// algorithm for removing line duplicates?
				for (ColliderComponent* c : o->GetColliders()) {
					if (c->coll != nullptr) {
						int startV = atVec;
						if (atVec + c->points.size() * 3 > 3 * vecLimit) {
							if (!reachedLimit)
								bug::out + bug::RED + "Line limit does not allow " + (atVec + c->points.size() * 3)+" points" + bug::end;
							reachedLimit = true;
							break;
						}else if (atInd+c->coll->quad.size()*8+c->coll->tri.size()*6>2*lineLimit) {
							if (!reachedLimit)
								bug::out + bug::RED + "Line limit does not allow "+((atInd + c->coll->quad.size() * 8 + c->coll->tri.size() * 6)/2)+" lines:"+bug::end;
							reachedLimit = true;
							break;
						}
						for (int i = 0; i < c->points.size(); i++) {
							glm::vec3 v = c->points[i];
							hitboxVec[atVec++] = v.x;
							hitboxVec[atVec++] = v.y;
							hitboxVec[atVec++] = v.z;
						}
						for (int i = 0; i < c->coll->quad.size()/4; i++) {
							hitboxInd[atInd++] = startV + c->coll->quad[i * 4 + 0];
							hitboxInd[atInd++] = startV + c->coll->quad[i * 4 + 1];
							hitboxInd[atInd++] = startV + c->coll->quad[i * 4 + 1];
							hitboxInd[atInd++] = startV + c->coll->quad[i * 4 + 2];
							hitboxInd[atInd++] = startV + c->coll->quad[i * 4 + 2];
							hitboxInd[atInd++] = startV + c->coll->quad[i * 4 + 3];
							hitboxInd[atInd++] = startV + c->coll->quad[i * 4 + 3];
							hitboxInd[atInd++] = startV + c->coll->quad[i * 4 + 0];
						}
						
						for (int i = 0; i < c->coll->tri.size() / 3; i++) {
							hitboxInd[atInd++] = atVec + c->coll->tri[i * 3 + 0];
							hitboxInd[atInd++] = atVec + c->coll->tri[i * 3 + 1];
							hitboxInd[atInd++] = atVec + c->coll->tri[i * 3 + 1];
							hitboxInd[atInd++] = atVec + c->coll->tri[i * 3 + 2];
							hitboxInd[atInd++] = atVec + c->coll->tri[i * 3 + 2];
							hitboxInd[atInd++] = atVec + c->coll->tri[i * 3 + 0];
						}
					}
				}
				// Clear the rest
				for (int i = atVec; i < vecLimit;i++) {
					hitboxVec[i] = 0;
				}
				for (int i = atInd; i < lineLimit*2; i++) {
					hitboxVec[i] = 0;
				}
				hitbox.SubVB(0, vecLimit*3, hitboxVec);
				hitbox.SubIB(0, lineLimit*2, hitboxInd);
				hitbox.Draw();
			}
		}

		/*
		renderer::SwitchBlendDepth(false);
		renderer::BindShader(MaterialType::ColorMat);
		
		for (GameObject& o : *oManager::GetObjects()) {
			if (o.renderMesh) {
				//if (o.meshData != nullptr) {
				//	if (o.meshData->material == MaterialType::ColorMat) {
						o.PreDraw();
				//	}
				//}
			}
		}
		renderer::BindShader(MaterialType::TextureMat);
		for (GameObject& o : *oManager::GetObjects()) {
			if (o.renderMesh) {
				if (o.meshData != nullptr) {
					if (o.meshData->material == MaterialType::TextureMat) {
						//std::cout << "What\n";
						o.Draw();
					}
				}
			}

		}
		renderer::BindShader(MaterialType::OutlineMat);
		glLineWidth(2.f);
		for (GameObject& o : *oManager::GetObjects()) {
			if (o.renderHitbox) {
				//if (o.collision.) { TODO: Check if collision has draw object?
				o.collision.Draw();
				//}
			}
		}
		if (oManager::GetPlayer()->renderHitbox/*&&GetPlayer()->freeCam) {
			oManager::GetPlayer()->collision.Draw();
		}
		for (GameObject* o : texRender) {
			o->Draw();
		}
		BindShader("terrain");
		if (loadedDim != nullptr) {
			BindTexture("blank");
			for (Chunk c : loadedDim->loadedChunks) {
				Location base;
				base.Translate(glm::vec3(c.x*(loadedDim->chunkSize), 0, c.z*(loadedDim->chunkSize)));
				ObjectTransform(base.mat());
				c.con.Draw();
			}
		}*/
		if (dManager::GetMenu() == Gameplay) {
			
		}
		iManager::RenderInterface();
	}
}