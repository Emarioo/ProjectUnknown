#include "gonpch.h"

#include "Engine.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

namespace engine {
	
	FrameBuffer depthBuffer;
	glm::mat4 lightProjection;
	FrameBuffer& GetDepthBuffer() {
		return depthBuffer;
	}
	glm::mat4& GetLightProj() {
		return lightProjection;
	}
	TriangleBuffer temp_;

	void Initialize() {
		InitRenderer();
		InitSound();
		InitInterface();
		ReadOptions();

		//std::cout << "Using " << glGetString(GL_RENDERER) << std::endl;

		float vMap[]{0,0,0,1,0,0,1,0,1,0,0,1};
		unsigned int iMap[]{0,1,2,2,3,0};

		temp_.Init(true,vMap,4*3,iMap,6);
		temp_.SetAttrib(0, 3, 3, 0);

		UpdateKeyboard(false);

		depthBuffer.Init();
		float near_plane = 1.f, far_plane = 20.5f;
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
	}
	void Uninitialize() {
		UninitSound();

	}

	int vecLimit = 500;
	int lineLimit = 3000;
	LineBuffer hitbox;
	float* hitboxVec;
	unsigned int* hitboxInd;

	void UpdateObjects(double delta) {
		// GetPlayer()->doMove = !GetCursorMode();// && !GetChatMode();

		// Additional Player Movement
		//engine::GetPlayer()->finalVelocity += GetPlayer()->Movement(delta);

		// Sort slowest velocity first
		for (int i = 0; i < GetObjects().size(); i++) {
			GameObject* o0 = GetObjects().at(i);

			o0->Update(delta);
			/*
			if (o0->collisionComponent.isActive) {
				for (int j = i + 1; j < GetObjects().size();j++) {
					GameObject* o1 = GetObjects().at(j);
					if (o1->collisionComponent.isActive) {
						if (o0->IsClose(o1)) {

							glm::vec3 newVelocity = o0->WillCollide(o1, delta);
							if (newVelocity != o0->velocity) { // Collision
								//bug::out < "Collide " < o1->name <bug::end;
								o0->velocity = newVelocity;
							}
						}
					}
				}
			}*/
			o0->position += o0->velocity * (float)delta;
		}

		//if (GetDimension() != nullptr)
		//	GetDimension()->CleanChunks();

		// Update dimension
	}
	void UpdateUI(double delta) {
		UpdateTimedFunc(delta);
		UpdateInterface(delta);
	}
	// If hitbox buffer is too small
	bool reachedLimit = false;
	void RenderRawObjects(double lag) {
		for (GameObject* o : GetObjects()) {
			if (o->renderComponent.model != nullptr) {
				for (int i = 0; i < o->renderComponent.model->meshes.size(); i++) {
					Mesh* mesh = o->renderComponent.model->meshes[i];
					if (mesh != nullptr) {
						SetTransform(o->renderComponent.matrix*
							glm::translate(o->velocity*(float)lag)* // special
							o->renderComponent.model->matrices[i]);
						mesh->Draw();
					}
				}
			}
		}
	}
	void RenderObjects(double lag) {
		// Update mesh transformations
		for (GameObject* o : GetObjects()) {
			if (o->renderMesh) {
				engine::Location body;
				body.Translate(o->position+ o->velocity * (float)lag);
				body.Rotate(o->rotation);
				body.Matrix(glm::mat4_cast(o->quaternion));
				o->renderComponent.matrix = body.mat();
			}
		}

		UpdateViewMatrix(lag);

		if (BindShader(ShaderType::Color)) {
			UpdateProjection();
			GetBoundShader()->SetVec3("uCamera", GetCamera()->position + GetCamera()->velocity * (float)lag);
			for (GameObject* o : GetObjects()) {
				if (!o->renderMesh)
					continue;
				if (o->renderComponent.model != nullptr) {
					BindLights(o->position);
					for (int i = 0; i < o->renderComponent.model->meshes.size(); i++) {
						Mesh* mesh = o->renderComponent.model->meshes[i];
						if (mesh != nullptr) {
							if (mesh->shaderType == ShaderType::Color) {// The shader might not be called here. so binding lights may be unecessary
								for (int i = 0; i < mesh->materials.size()&&i<4;i++) {// Maximum of 4 materials
									PassMaterial(i,mesh->materials[i]);
									//bug::out < i < bug::end;
								}
								SetTransform(o->matrix*o->renderComponent.matrix*o->renderComponent.model->matrices[i]);
								mesh->Draw();
							}
						} else {
							if (!o->renderComponent.hasError) {
								bug::out < bug::RED < "Mesh in Model is nullptr\n";
								o->renderComponent.hasError = true;
							}
						}
					}
				}
			}
		}
		
		if (BindShader(ShaderType::ColorBone)) {
			UpdateProjection();
			GetBoundShader()->SetVec3("uCamera", GetCamera()->position+GetCamera()->velocity*(float)lag);
			for (GameObject* o : GetObjects()) {
				if (!o->renderMesh)
					continue;
				if (o->renderComponent.model != nullptr) {
					//bug::outs < "Update " < o->name<"\n";
					BindLights(o->position);
					if (o->renderComponent.model->armature != nullptr) {
						if (!o->renderComponent.model->armature->hasError) {
							//bug::outs < o->renderComponent.animator.enabledAnimations["georgeBoneAction"].frame < "\n";
							int count = o->renderComponent.model->armature->bones.size();
							std::vector<glm::mat4> mats(count);
							o->renderComponent.GetArmatureTransforms(mats);
							for (int i = 0; i < count; i++) {
								//bug::outs < mats[i] < "\n";
								GetBoundShader()->SetMatrix("uBoneTransforms[" + std::to_string(i) + "]", mats[i]);
							}
						}
					}
					for (int i = 0; i < o->renderComponent.model->meshes.size(); i++) {
						Mesh* mesh = o->renderComponent.model->meshes[i];
						if (mesh != nullptr) {
							if (mesh->shaderType == ShaderType::ColorBone) {// The shader might not be called here. so binding lights may be unecessary
								for (int i = 0; i < mesh->materials.size() && i < 4; i++) {// Maximum of 4 materials
									PassMaterial(i, mesh->materials[i]);
								}
								SetTransform(o->matrix * o->renderComponent.matrix * o->renderComponent.model->matrices[i]);
								mesh->Draw();
							}
						} else {
							if (!o->renderComponent.hasError) {
								bug::out < bug::RED < "Mesh in Model is nullptr\n";
								o->renderComponent.hasError = true;
							}
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
		}*/
		
		if (BindShader(ShaderType::Outline)) {
			UpdateProjection();
			SetTransform(glm::mat4(1));
			glLineWidth(2.f);
			/*
			GameObject* n = GetObjectByName("Player");
			if (n != nullptr) {
				CollisionComponent& c = n->collisionComponent;

				int atVec = 0;
				int atInd = 0;
				int startV = 0;

				bug::out < c.points.size() <" "< c.coll->quad.size() < bug::end;
				for (int i = 0; i < c.points.size(); i++) {
					glm::vec3 v = c.points[i];
					hitboxVec[atVec++] = v.x;
					hitboxVec[atVec++] = v.y;
					hitboxVec[atVec++] = v.z;
				}
				for (int i = 0; i < c.coll->quad.size() / 4; i++) {
					hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 0];
					hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 1];
					hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 1];
					hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 2];
					hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 2];
					hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 3];
					hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 3];
					hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 0];
				}
				/*
				for (int i = 0; i < c.coll->tri.size() / 3; i++) {
					hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 0];
					hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 1];
					hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 1];
					hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 2];
					hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 2];
					hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 0];
				}
				// Clear the rest
				for (int i = atVec; i < vecLimit * 3; i++) {
					hitboxVec[i] = 0;
				}
				for (int i = atInd; i < lineLimit * 2; i++) {
					hitboxVec[i] = 0;
				}

				hitbox.SubVB(0, vecLimit * 3, hitboxVec);
				hitbox.SubIB(0, lineLimit * 2, hitboxInd);
				hitbox.Draw();
			}*/

			glm::vec3 hitboxColor = { 0,1,0 };
			for (GameObject* o : GetObjects()) {// Draws a hitbox for every object by thowing all the lines from colliders into a vertex buffer
				if (o->renderHitbox && o->collisionComponent.isActive) {
					int atVec = 0;
					int atInd = 0;
					CollisionComponent& c = o->collisionComponent;
					// algorithm for removing line duplicates?
					// Adding hitboxes together?
					int startV = atVec;
					
					if (atVec + c.points.size()*6 > 6*vecLimit) {
						if (!reachedLimit)
							bug::out < bug::RED < "Line limit does not allow " < (atVec+c.points.size() * 6)<"/"<(6*vecLimit) < " points" < bug::end;
						reachedLimit = true;
						continue;
					} else if (atInd + c.coll->quad.size() * 4 + c.coll->tri.size() * 3 > lineLimit) {
						if (!reachedLimit)
							bug::out < bug::RED < "Line limit does not allow " < (atInd + c.coll->quad.size() * 4 + c.coll->tri.size() * 3)<"/"<(lineLimit) < " lines:" < bug::end;
						reachedLimit = true;
						continue;
					}
					for (int i = 0; i < c.points.size(); i++) {
						glm::vec3 v = c.points[i];
						hitboxVec[atVec++] = v.x;
						hitboxVec[atVec++] = v.y;
						hitboxVec[atVec++] = v.z;
						hitboxVec[atVec++] = hitboxColor.x;
						hitboxVec[atVec++] = hitboxColor.y;
						hitboxVec[atVec++] = hitboxColor.z;
					}
					for (int i = 0; i < c.coll->quad.size() / 4; i++) {
						hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 0];
						hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 1];
						hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 1];
						hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 2];
						hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 2];
						hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 3];
						hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 3];
						hitboxInd[atInd++] = startV + c.coll->quad[i * 4 + 0];
					}
					/*
					for (int i = 0; i < c.coll->tri.size() / 3; i++) {
						hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 0];
						hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 1];
						hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 1];
						hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 2];
						hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 2];
						hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 0];
					}*/
					// Clear the rest - how necessary is this?
					for (int i = atVec; i < vecLimit*6; i++) {
						hitboxVec[i] = 0;
					}
					for (int i = atInd; i < lineLimit * 2; i++) {
						hitboxVec[i] = 0;
					}
					hitbox.ModifyVertices(0, vecLimit * 6, hitboxVec);
					hitbox.ModifyIndices(0, lineLimit * 2, hitboxInd);
					hitbox.Draw();
				}
				if (o->renderComponent.model!=nullptr) {
					if (o->renderComponent.model->armature != nullptr) {
						if (!o->renderComponent.model->armature->hasError) {
							//bug::outs < o->renderComponent.animator.enabledAnimations["georgeBoneAction"].frame < "\n";
							Armature* arm = o->renderComponent.model->armature;
							int count = arm->bones.size();
							//std::cout << count << "\n";
							std::vector<glm::mat4> mats(count);
							o->renderComponent.GetArmatureTransforms(mats);
							/*for (int i = 0; i < count; i++) {
								//bug::outs < mats[i] < "\n";
								GetBoundShader()->SetMatrix("uBoneTransforms[" + std::to_string(i) + "]", mats[i]);
							}*/

							int atVec = 0;
							int atInd = 0;
							int startV = atVec;

							// algorithm for removing line duplicates?
							if (atVec + count*12 > 6*vecLimit) {
								if (!reachedLimit)
									bug::out < bug::RED < "Line limit does not allow " < (atVec + count * 12) < "/" < (6 * vecLimit) < " points" < bug::end;
								reachedLimit = true;
								continue;
							}
							else if (atInd + count > lineLimit) {
								if (!reachedLimit)
									bug::out < bug::RED < "Line limit does not allow " < (atInd + count) < "/" < (lineLimit) < " lines:" < bug::end;
								reachedLimit = true;
								continue;
							}
							for (int i = 0; i < arm->bones.size(); i++) {
								glm::vec3 v = (mats[i]* o->renderComponent.matrix)[3];
								hitboxVec[atVec++] = v.x;
								hitboxVec[atVec++] = v.y;
								hitboxVec[atVec++] = v.z;
								hitboxVec[atVec++] = hitboxColor.x;
								hitboxVec[atVec++] = hitboxColor.y;
								hitboxVec[atVec++] = hitboxColor.z;

								hitboxInd[atInd++] = i;
								hitboxInd[atInd++] = i+1;
							}
							/*
							for (int i = 0; i < c.coll->tri.size() / 3; i++) {
								hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 0];
								hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 1];
								hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 1];
								hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 2];
								hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 2];
								hitboxInd[atInd++] = atVec + c.coll->tri[i * 3 + 0];
							}*/
							// Clear the rest - how necessary is this?
							for (int i = atVec; i < vecLimit * 6; i++) {
								hitboxVec[i] = 0;
							}
							for (int i = atInd; i < lineLimit * 2; i++) {
								hitboxInd[i] = 0;
							}
							hitbox.ModifyVertices(0, vecLimit * 6, hitboxVec);
							hitbox.ModifyIndices(0, lineLimit * 2, hitboxInd);
							hitbox.Draw();
						}
					}
				}
			}
		}
		/*
		Mesh* lightCube = GetMeshAsset("LightCube");
		if (lightCube!=nullptr) {
			if (BindShader(ShaderLight)) {
				PassProjection();
				//bug::out < "ea" < GetLights().size()< bug::end;
				for (Light* l : GetLights()) {
					//if (l->lightType != LightType::Direction) {
						//bug::out < "rend light" < bug::end;
						GetShader(ShaderCurrent)->SetVec3("uLightColor", l->diffuse);
						PassTransform(glm::translate(l->position));
						lightCube->Draw();
					//}
				}
			}
		}
		*/
		/*
		if (BindShader(ShaderType::Terrain)) {
			SetProjection();
			Dimension* dim = GetDimension();
			if (dim != nullptr) {
				BindTexture(0,"blank");
				for (Chunk c : dim->loadedChunks) {
					Location base;
					base.Translate(glm::vec3(c.x * (dim->chunkSize), 0, c.z * (dim->chunkSize)));
					PassTransform(base.mat());
					c.con.Draw();
				}
			}
		}*/
	}
	void RenderUI(double lag) {
		RenderInterface(lag);
	}

	void Start(std::function<void(double)> update, std::function<void(double)> render,double fps){
		hitbox.Init(true, nullptr, 6 * vecLimit, nullptr, 2 * lineLimit);
		hitbox.SetAttrib(0, 3, 6, 0);
		hitbox.SetAttrib(1, 3, 6, 3);
		hitboxVec = new float[vecLimit * 6];// {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1}; not using color
		hitboxInd = new unsigned int[lineLimit * 2];// {0, 1, 0, 2, 0, 3};

		/*
		// Loop
		int lastTime = GetTime();
		int lastDTime = lastTime;
		int FPS = 0;
		int tickSpeed = 0;
		float delta;

		while (RenderRunning()) {
			int now = GetTime();
			if (now - lastTime > 1000) {
				lastTime = now;
				SetWindowTitle(("Project Unknown  " + std::to_string(FPS) + " fps").c_str());
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
		}*/
		// Other
		int FPS = fps; // variable is also used in loop

		double previous = GetPlayTime();
		double lag = 0.0;
		double MS_PER_UPDATE = 1. / FPS;
		double lastSecond=previous;
		while (RenderRunning()) {
			double current = GetPlayTime();
			double elapsed = current - previous;
			//std::cout << elapsed << std::endl;
			previous = current;
			lag += elapsed;

			while (lag >= MS_PER_UPDATE)
			{
				update(MS_PER_UPDATE);
				lag -= MS_PER_UPDATE;
				FPS++;
				if (current-lastSecond>1) {
					//std::cout << (current - lastSecond) << std::endl;
					lastSecond = current;
					SetWindowTitle(("Project Unknown  " + std::to_string(FPS) + " fps").c_str());
					FPS = 0;
				}
			}

			render(lag);
			glfwSwapBuffers(GetWindow());
			glfwPollEvents();
		}
		// remove buffer and stuff or not?
		RenderTermin();

	}
}