#include "Gamecore.h"

#include "Objects/MagicStaff.h"

#include "Utility/Keyboard.h"

#include <ctime>
#include <chrono>

#include "Objects/Cube.h"
#include "Objects/NetPlayer.h"
#include "Objects/Tutorial.h"
#include "Objects/Goblin.h"
#include "Objects/OTemplate.h"

#include "Chat.h"
#include "Utility/GameOptions.h"

#include "Managers/InterfaceManager.h"
#include "Managers/ObjectManager.h"

#include "StartMenu.h"

//#include "MagicEditor.h"

namespace gamecore {
	int vecLimit = 100;
	int lineLimit = 100;
	BufferContainer hitbox;
	float* hitboxVec;
	unsigned int* hitboxInd;
	void LoadGameData() {// ALL game assets should be added here(Not interface stuff)
		renderer::AddTexture("magicstaff_staff", "assets/textures/magicstaff_staff");
		renderer::AddTexture("magicstaff_fireball", "assets/textures/magicstaff_fireball");
		dManager::AddAnim("MagicStaffIdle");
		dManager::AddMesh("staff");
		dManager::AddMesh("fireball");

		dManager::AddAnim("PlayerIdle");
		dManager::AddMesh("Body");
		dManager::AddMesh("Head");
		dManager::AddMesh("RUArm");
		dManager::AddMesh("RLArm");

		dManager::AddAnim("goblin_slash");
		dManager::AddBone("goblin_skeleton");
		dManager::AddMesh("goblin_body");

		renderer::AddTexture("noise", "assets/textures/noise");
		dManager::AddMesh("Parkour");
		dManager::AddColl("Parkour");

		dManager::AddMesh("Floor");
		dManager::AddColl("Floor");

		dManager::AddMesh("VertexColor");
		dManager::AddColl("VertexColor");
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

		dManager::SetGameState(GameState::play);// Temporary

		dManager::Init();
		iManager::Init();
		oManager::Init();
		
		oManager::GetPlayer()->SetPosition(0, 2, 0);

		startMenu::Init();

		//InitEditor();
		UpdateKeyboard(false);
		//InitChat(&updateObjects);

		iManager::SetPauseMode(true);

		MetaStrip pMeta(Velocity, Pos);
		pMeta.bools[0] = true;
		oManager::GetPlayer()->metaData.AddMeta(pMeta);

		oManager::GetPlayer()->renderHitbox = true;
		
		//oManager::AddObject(new MagicStaff(0, 0, -10));
		oManager::AddObject(new Goblin(0, 0, -10));
		/*Tutorial* tut = new Tutorial(0, 0, 0);
		tut->renderHitbox = true;
		oManager::AddObject(tut);
		/*
		OTemplate* temp = new OTemplate(0, 5, 0);
		temp->renderHitbox = true;
		oManager::AddObject(temp);
		*/
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
				//Update(1.f/60);//delta);
				Update(delta);
				
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
		if (dManager::GetGameState() == GameState::menu) {
			// Game Menu Update

		} else if (dManager::GetGameState() == GameState::play) {
			if (renderer::HasFocus() && !iManager::IsPauseMode()) {
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

					if (o0->GetColliders().size() > 0) {
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
				}
				if (!oManager::GetPlayer()->freeCam) {
					renderer::GetCamera()->position = oManager::GetPlayer()->position;
				} else if (oManager::GetPlayer()->freeCam && oManager::GetPlayer()->moveCam) {

				} else {

				}
				renderer::UpdateProj();

				if (oManager::GetDimension() != nullptr)
					oManager::GetDimension()->CleanChunks();

				// Update dimension

				// Update mesh transformations should maybe be moved to render
				for (GameObject* o : *oManager::GetObjects()) {
					if (o->renderMesh) {
						o->PreComponents();
					}
				}
			}
		}
		dManager::Update(delta);
		iManager::UpdateInterface(delta);
	}
	bool reachedLimit = false;
	void Render() {
		if (dManager::GetGameState() == GameState::menu) {

		} else if(dManager::GetGameState() == GameState::play) {
			// Run preComponents here?
			renderer::SwitchBlendDepth(false);
			renderer::BindShader(MaterialType::ColorMat);
			for (RenderComponent* m : *dManager::GetRenderComponents()) {
				for (int i = 0; i < m->meshes.size(); i++) {
					MeshData* mesh = m->meshes[i];
					if (mesh != nullptr) {
						if (mesh->material == MaterialType::ColorMat) {
							renderer::DrawMesh(mesh, m->matrices[i]);
						}
					} else {
						if (!m->hasError) {
							bug::out < bug::RED < "MeshData in MeshComponent is nullptr\n";
							m->hasError = true;
						}
					}
				}
			}
			renderer::BindShader(MaterialType::TextureMat);
			for (RenderComponent* m : *dManager::GetRenderComponents()) {
				for (int i = 0; i < m->meshes.size(); i++) {
					MeshData* mesh = m->meshes[i];
					if (mesh != nullptr) {
						if (mesh->material == MaterialType::TextureMat) {
							renderer::DrawMesh(mesh, m->matrices[i]);
						}
					}
				}
			}
			renderer::BindShader(MaterialType::AnimationMat);
			for (RenderComponent* m : *dManager::GetRenderComponents()) {
				if (m->bone != nullptr) {
					int count = m->bone->bone->count;
					glm::mat4* mats = new glm::mat4[count];
					m->bone->GetBoneTransforms(mats);
					for (int i = 0; i < count; i++) {
						renderer::GetShader(MaterialType::AnimationMat)->SetUniformMat4fv("uBoneTransforms[" + i + std::string("]"), mats[i]);
					}
					delete mats;
				}
				for (int i = 0; i < m->meshes.size(); i++) {
					MeshData* mesh = m->meshes[i];
					if (mesh != nullptr) {
						if (mesh->material == MaterialType::AnimationMat) {
							renderer::DrawMesh(mesh, m->matrices[i]);// Could be causing problems by not giving it an empty matrix
						}
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
									bug::out < bug::RED < "Line limit does not allow " < (atVec < c->points.size() * 3) < " points" < bug::end;
								reachedLimit = true;
								break;
							} else if (atInd + c->coll->quad.size() * 8 + c->coll->tri.size() * 6 > 2 * lineLimit) {
								if (!reachedLimit)
									bug::out < bug::RED < "Line limit does not allow " < ((atInd < c->coll->quad.size() * 8 < c->coll->tri.size() * 6) / 2) < " lines:" < bug::end;
								reachedLimit = true;
								break;
							}
							for (int i = 0; i < c->points.size(); i++) {
								glm::vec3 v = c->points[i];
								hitboxVec[atVec++] = v.x;
								hitboxVec[atVec++] = v.y;
								hitboxVec[atVec++] = v.z;
							}
							for (int i = 0; i < c->coll->quad.size() / 4; i++) {
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
					for (int i = atVec; i < vecLimit; i++) {
						hitboxVec[i] = 0;
					}
					for (int i = atInd; i < lineLimit * 2; i++) {
						hitboxVec[i] = 0;
					}
					hitbox.SubVB(0, vecLimit * 3, hitboxVec);
					hitbox.SubIB(0, lineLimit * 2, hitboxInd);
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
		}
		iManager::RenderInterface();
	}
}