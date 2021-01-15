#include "Engine.h"

#include <chrono>

namespace engine {
	
	FrameBuffer depthBuffer;
	glm::mat4 lightProjection;
	FrameBuffer& GetDepthBuffer() {
		return depthBuffer;
	}
	glm::mat4& GetLightProj() {
		return lightProjection;
	}
	void Initialize() {
		InitRenderer();
		InitInterface();
		InitPlayer();
		UpdateKeyboard(false);

		depthBuffer.Init();
		float near_plane = 1.f, far_plane = 7.5f;
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
	}

	int vecLimit = 100;
	int lineLimit = 100;
	BufferContainer hitbox;
	float* hitboxVec;
	unsigned int* hitboxInd;

	int GetTime() {
		return std::chrono::system_clock::now().time_since_epoch().count() / 10000;
	}

	void UpdateObjects(float delta) {
		GetPlayer()->doMove = !GetCursorMode();// && !GetChatMode();

		for (int i = 0; i < GetObjects().size(); i++) {
			GetObjects().at(i)->velocity = glm::vec3(0, 0, 0);
		}
		// Additional Player Movement
		GetPlayer()->velocity += GetPlayer()->Movement(delta);

		// Sort slowest velocity first
		for (int i = 0; i < GetObjects().size(); i++) {
			GameObject* o0 = GetObjects().at(i);
			MetaStrip* ms = o0->metaData.GetMeta(Velocity, Pos, None);

			if (ms != nullptr) {
				o0->velocity += glm::vec3(ms->floats[0], ms->floats[1], ms->floats[2]);
			}

			if (o0->GetColliders().size() > 0) {
				// If velocity has changed then go trough the list of all objects again
				for (int j = i + 1; j < GetObjects().size(); j++) {
					GameObject* o1 = GetObjects().at(j);
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
		if (!GetPlayer()->freeCam) {
			GetCamera()->position = GetPlayer()->position;
		} else if (GetPlayer()->freeCam && GetPlayer()->moveCam) {

		} else {

		}

		UpdateViewMatrix();

		if (GetDimension() != nullptr)
			GetDimension()->CleanChunks();

		// Update dimension

		// Update mesh transformations should maybe be moved to render
		for (GameObject* o : GetObjects()) {
			if (o->renderMesh) {
				o->PreComponents();
			}
		}
	}
	void UpdateUI(float delta) {
		UpdateTimedFunc(delta);
		UpdateInterface(delta);
	}
	// If hitbox buffer is too small
	bool reachedLimit = false;
	void RenderRawObjects() {
		for (GameObject* o : GetObjects()) {
			for (int i = 0; i < o->renderComponent.meshes.size(); i++) {
				MeshData* mesh = o->renderComponent.meshes[i];
				if (mesh != nullptr) {
					PassTransform(o->renderComponent.matrices[i]);
					mesh->Draw();
				}
			}
		}
	}
	void RenderObjects() {
		// Run preComponents here?
		if (BindShader(ShaderColor)) {
			PassProjection();
			GetShader()->SetVec3("uCamera", GetCamera()->position);
			for (GameObject* o : GetObjects()) {
				BindLights(o->position);
				for (int i = 0; i < o->renderComponent.meshes.size(); i++) {
					MeshData* mesh = o->renderComponent.meshes[i];
					if (mesh != nullptr) {
						if (mesh->shaderType == ShaderColor) {// The shader might not be called here. so binding lights may be unecessary
							PassMaterial(mesh->material);
							PassTransform(o->renderComponent.matrices[i]);
							mesh->Draw();
						}
					} else {
						if (!o->renderComponent.hasError) {
							bug::out < bug::RED < "MeshData in MeshComponent is nullptr\n";
							o->renderComponent.hasError = true;
						}
					}
				}
			}
		}
		/*
		if (BindShader(ShaderUV)) {
			UpdateViewMatrix();
			for (GameObject* o : GetObjects()) {
				for (int i = 0; i < o->renderComponent.meshes.size(); i++) {
					MeshData* mesh = o->renderComponent.meshes[i];
					if (mesh != nullptr) {
						if (mesh->shaderType == ShaderUV) {
							//BindTexture(mesh->texture);
							PassTransform(o->renderComponent.matrices[i]);
							mesh->Draw();
						}
					}
				}
			}
		}
		if (BindShader(ShaderColorBone)) {
			UpdateViewMatrix();
			for (GameObject* o : GetObjects()) {
				if (o->renderComponent.bone.enabled) {
					int count = o->renderComponent.bone.bone->bones.size();
					std::vector<glm::mat4> mats(count);
					o->renderComponent.bone.GetBoneTransforms(mats);
					for (int i = 0; i < count; i++) {
						GetShader(ShaderColorBone)->SetMatrix("uBoneTransforms[" + std::to_string(i) + "]", mats[i]);
					}
				}
				for (int i = 0; i < o->renderComponent.meshes.size(); i++) {
					MeshData* mesh = o->renderComponent.meshes[i];
					if (mesh != nullptr) {
						if (mesh->shaderType == ShaderColorBone) {
							PassTransform(o->renderComponent.matrices[i]);
							mesh->Draw();
						}
					}
				}
			}
		}
		if (BindShader(ShaderUVBone)) {
			UpdateViewMatrix();
			for (GameObject* o : GetObjects()) {
				if (o->renderComponent.bone.enabled) {
					int count = o->renderComponent.bone.bone->bones.size();
					std::vector<glm::mat4> mats(count);
					o->renderComponent.bone.GetBoneTransforms(mats);
					for (int i = 0; i < count; i++) {
						GetShader(ShaderCurrent)->SetMatrix("uBoneTransforms[" + std::to_string(i) + "]", mats[i]);
					}
				}
				for (int i = 0; i < o->renderComponent.meshes.size(); i++) {
					MeshData* mesh = o->renderComponent.meshes[i];
					if (mesh != nullptr) {
						if (mesh->shaderType == ShaderUVBone) {
							PassTransform(o->renderComponent.matrices[i]);
							mesh->Draw();
						}
					}
				}
			}
		}
		if (BindShader(ShaderOutline)) {
			UpdateViewMatrix();
			PassTransform(glm::mat4(1));
			PassColor(1, 1, 1, 1);
			glLineWidth(2.f);
			for (GameObject* o : GetObjects()) {// Draws a hitbox for every object by thowing all the lines from colliders into a vertex buffer
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
		}*/
		MeshData* lightCube = GetMeshAsset("LightCube");
		if (lightCube!=nullptr) {
			if (BindShader(ShaderLight)) {
				PassProjection();
				for (Light* l : GetLights()) {
					if (l->lightType != LightType::Direction) {
						GetShader(ShaderCurrent)->SetVec3("uLightColor", l->diffuse);
						PassTransform(glm::translate(l->position));
						lightCube->Draw();
					}
				}
			}
		}
		/*
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
	void RenderUI() {
		RenderInterface();
	}

	void Start(std::function<void(float)> update, std::function<void()> render){
		hitbox.type = 1;
		hitbox.Setup(true, nullptr, 3 * vecLimit, nullptr, 2 * lineLimit);
		hitbox.SetAttrib(0, 3, 3, 0);
		hitboxVec = new float[vecLimit * 3];
		hitboxInd = new unsigned int[lineLimit * 2];

		// Loop
		int lastTime = GetTime();
		int lastDTime = lastTime;
		int FPS = 0;
		int tickSpeed = 0;
		float delta;

		/*
		Option to lock fps
		*/
		while (RenderRunning()) {
			int now = GetTime();
			if (now - lastTime > 1000) {
				lastTime = now;
				SetWindowTitle(("Reigai Dimension  " + std::to_string(FPS) + " fps").c_str());
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
				update(delta);

				render();
			}


			glfwSwapBuffers(GetWindow());
			glfwPollEvents();
		}
		// remove buffer and stuff or not?
		RenderTermin();

	}
}