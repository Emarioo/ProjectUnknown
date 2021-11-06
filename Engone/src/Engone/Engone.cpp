#include "gonpch.h"

#include "Engone.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

namespace engone {

	FrameBuffer depthBuffer;
	glm::mat4 lightProjection;
	FrameBuffer& GetDepthBuffer()
	{
		return depthBuffer;
	}
	glm::mat4& GetLightProj()
	{
		return lightProjection;
	}
	TriangleBuffer temp_;

	static glm::mat4 projMatrix;
	static glm::mat4 viewMatrix;
	static float fov, zNear, zFar;
	void SetProjection(float ratio)
	{
		projMatrix = glm::perspective(fov, ratio, zNear, zFar);
	}
	void UpdateViewMatrix(double lag)
	{
		//GetCamera()->rotation.y += 0.001;
		//std::cout << GetCamera()->rotation.x <<" "<< GetCamera()->rotation.y <<" "<< GetCamera()->rotation.z << "\n";
		viewMatrix = glm::inverse(
			glm::translate(glm::mat4(1.0f), GetCamera()->position)*
			glm::rotate(GetCamera()->rotation.y, glm::vec3(0, 1, 0)) *
			glm::rotate(GetCamera()->rotation.x, glm::vec3(1, 0, 0))
		);
	}
	void UpdateProjection(Shader* shader)
	{
		if (shader != nullptr)
			shader->SetMatrix("uProj", projMatrix * viewMatrix);
	}

	static const std::string objectSource = {
#include "Shaders/object.glsl"
	};
	static const std::string armatureSource = {
#include "Shaders/armature.glsl"
	};
	static const std::string outlineSource = {
#include "Shaders/outline.glsl"
	};

	static int lastMouseX=-1, lastMouseY=-1;
	// Move somewhere else
	static double cameraSensitivity = 0.1;
	EventType FirstPerson(Event& e)
	{
		if (lastMouseX != -1) {
			if (IsCursorLocked() && GetCamera() != nullptr) {
				GetCamera()->rotation.y -= (e.mx - lastMouseX) * (3.14159 / 360) * cameraSensitivity;
				GetCamera()->rotation.x -= (e.my - lastMouseY) * (3.14159 / 360) * cameraSensitivity;
			}
		}
		lastMouseX = e.mx;
		lastMouseY = e.my;
		return EventType::None;
	}

	void InitEngone()
	{
		InitRenderer();

		AddAsset<Shader>("object", new Shader(objectSource, true));
		AddAsset<Shader>("armature", new Shader(armatureSource, true));
		AddAsset<Shader>("outline", new Shader(outlineSource, true));

		DebugInit();
		InitEvents(GetWindow());
		InitEvents();

		InitGUI();
		ReadOptions();

		console::InitConsole(120,50);

		AddListener(new Listener(EventType::Move|EventType::GLFW, FirstPerson));

		//std::cout << "Using " << glGetString(GL_RENDERER) << std::endl;

		float vMap[]{0,0,0,1,0,0,1,0,1,0,0,1};
		unsigned int iMap[]{0,1,2,2,3,0};

		temp_.Init(true,vMap,4*3,iMap,6);
		temp_.SetAttrib(0, 3, 3, 0);

		//UpdateKeyboard(false);

		depthBuffer.Init();
		float near_plane = 1.f, far_plane = 20.5f;
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);

		fov = 90.f;
		zNear = 0.1f;
		zFar = 400.f;
		SetProjection((float)Width() / Height());
	}
	void ExitEngone()
	{

	}

	int vecLimit = 500;
	int lineLimit = 3000;
	LineBuffer hitbox;
	float* hitboxVec;
	unsigned int* hitboxInd;

	void UpdateObjects(double delta) 
	{
		// GetPlayer()->doMove = !GetCursorMode();// && !GetChatMode();

		// Additional Player Movement
		//engine::GetPlayer()->finalVelocity += GetPlayer()->Movement(delta);

		// Sort slowest velocity first
		for (int i = 0; i < GetObjects().size(); i++) 
		{
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
	void UpdateUI(double delta) 
	{
		UpdateTimedFunc(delta);
		UpdatePanels(delta);
	}
	// If hitbox buffer is too small
	bool reachedLimit = false;
	void RenderRawObjects(Shader* shader, double lag) 
	{
		for (GameObject* o : GetObjects()) {
			/*
			if (o->renderComponent.model != nullptr) {
				for (int i = 0; i < o->renderComponent.model->meshes.size(); i++) {
					//std::cout << " okay2\n";
					Mesh* mesh = o->renderComponent.model->meshes[i];
					if (mesh != nullptr) {
						//std::cout << o->GetName()<<" okay3\n";
						shader->SetMatrix("uTransform", o->renderComponent.matrix);
						/*  *
						glm::translate(o->velocity * (float)lag)* // special
							o->renderComponent.model->matrices[i]
						/*
						SetTransform(o->renderComponent.matrix*
							glm::translate(o->velocity*(float)lag)* // special
							o->renderComponent.model->matrices[i]);
						
						mesh->Draw();
					}
				}
			}
			*/
		}
	}
	void RenderObjects(double lag) 
	{
		// Update mesh transformations
		for (GameObject* o : GetObjects()) {
			if (o->renderMesh) {
				Location body;
				body.Translate(o->position+ o->velocity * (float)lag);
				body.Rotate(o->rotation);
				body.Matrix(glm::mat4_cast(o->quaternion));
				o->renderComponent.matrix = body.mat();
			}
		}

		SwitchBlendDepth(false);
		UpdateViewMatrix(lag);

		Shader* shader = GetAsset<Shader>("object");

		if (!shader->error)
		{
			shader->Bind();

			UpdateProjection(shader);
			shader->SetVec3("uCamera", GetCamera()->position + GetCamera()->velocity * (float)lag);
			/*
			MeshAsset* as = GetAsset<MeshAsset>("Leaf");
			if (as != nullptr) {
				for (int i = 0; i < as->materials.size();i++) {
					
					//std::cout << as->materials[i]->diffuse_color[0] << " " << as->materials[i]->diffuse_map << "\n";
					PassMaterial(shader,i,as->materials[i]);
				}
				shader->SetMatrix("uTransform", glm::mat4(1));
				as->buffer.Draw();
			}
			*/

			for (GameObject* o : GetObjects())
			{
				if (o->renderComponent.model != nullptr) 
				{
					BindLights(shader,o->position);
					
					// Get individual transforms
					int count = o->renderComponent.model->instances.size();
					std::vector<glm::mat4> mats(count);
					o->renderComponent.GetInstanceTransforms(mats);

					// Draw instances
					for (int i = 0; i < o->renderComponent.model->instances.size(); i++) 
					{
						AssetInstance& instance = o->renderComponent.model->instances[i];
						if (instance.asset->type == AssetType::Mesh) {
							MeshAsset* asset = instance.asset->cast<MeshAsset>();
							if (asset->meshType == MeshAsset::MeshType::Normal) {
								for (int j = 0; j < asset->materials.size() && j < 4; j++)// Maximum of 4 materials
								{
									PassMaterial(shader, j, asset->materials[j]);
								}
								shader->SetMatrix("uTransform", o->renderComponent.matrix * mats[i]);
								asset->buffer.Draw();
							}
						}
					}
				}
			}
		}
		shader = GetAsset<Shader>("armature");
		if (!shader->error)
		{
			shader->Bind();
			UpdateProjection(shader);
			shader->SetVec3("uCamera", GetCamera()->position+GetCamera()->velocity*(float)lag);
			for (GameObject* o : GetObjects()) 
			{
				/*
				if (o->renderComponent.model != nullptr)
				{
					//bug::outs < "Update " < o->name<"\n";
					BindLights(shader,o->position);
					if (o->renderComponent.model->armature != nullptr) 
					{
						if (!o->renderComponent.model->armature->hasError) 
						{
							//bug::outs < o->renderComponent.animator.enabledAnimations["georgeBoneAction"].frame < "\n";
							int count = o->renderComponent.model->armature->bones.size();
							std::vector<glm::mat4> mats(count);
							o->renderComponent.GetArmatureTransforms(mats);
							for (int i = 0; i < count; i++) 
							{
								//bug::outs < mats[i] < "\n";
								shader->SetMatrix("uBoneTransforms[" + std::to_string(i) + "]", mats[i]);
							}
						}
					}
					for (int i = 0; i < o->renderComponent.model->meshes.size(); i++) 
					{
						Mesh* mesh = o->renderComponent.model->meshes[i];
						if (mesh != nullptr) 
						{
							if (mesh->shaderType == "armature") 
							{// The shader might not be called here. so binding lights may be unecessary
								for (int i = 0; i < mesh->materials.size() && i < 4; i++) 
								{// Maximum of 4 materials
									PassMaterial(shader,i, mesh->materials[i]);
								}
								shader->SetMatrix("uTransform",o->renderComponent.matrix * o->renderComponent.model->matrices[i]);
								mesh->Draw();
							}
						} 
						else
						{
							if (!o->renderComponent.hasError) 
							{
								bug::out < bug::RED < "Mesh in Model is nullptr\n";
								o->renderComponent.hasError = true;
							}
						}
					}
				}
				*/
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
		shader = GetAsset<Shader>("outline");
		if (shader!=nullptr)
		{
			shader->Bind();
			UpdateProjection(shader);
			shader->SetMatrix("uTransform",glm::mat4(1));
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

#if gone
			glm::vec3 hitboxColor = { 0,1,0 };
			for (GameObject* o : GetObjects())
			{// Draws a hitbox for every object by thowing all the lines from colliders into a vertex buffer
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
				if (false&&o->renderComponent.model!=nullptr)
				{
					if (o->renderComponent.model->armature != nullptr)
					{
						if (!o->renderComponent.model->armature->hasError) {
							//bug::outs < o->renderComponent.animator.enabledAnimations["georgeBoneAction"].frame < "\n";
							Armature* arm = o->renderComponent.model->armature;
							int count = arm->bones.size();
							std::vector<glm::mat4> mats(count);
							o->renderComponent.GetArmatureTransforms(mats);
							for (int i = 0; i < count; i++) {
								//bug::outs < mats[i] < "\n";
								shader->SetMatrix("uBoneTransforms[" + std::to_string(i) + "]", mats[i]);
							}

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
								glm::vec3 v = mats[i][3];
								hitboxVec[atVec++] = v.x;
								hitboxVec[atVec++] = v.y;
								hitboxVec[atVec++] = v.z;
								hitboxVec[atVec++] = hitboxColor.x;
								hitboxVec[atVec++] = hitboxColor.y;
								hitboxVec[atVec++] = hitboxColor.z;
							}
							/*
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
								hitboxVec[i] = 0;
							}
							hitbox.ModifyVertices(0, vecLimit * 6, hitboxVec);
							hitbox.ModifyIndices(0, lineLimit * 2, hitboxInd);
							hitbox.Draw();
						}
					}
				}
			}
#endif
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
		RenderPanels();
	}
	static std::vector<TimedFunc> functions;
	void AddTimedFunction(std::function<void()> func, float time)
	{
		functions.push_back(TimedFunc(func, time));
	}
	void UpdateTimedFunc(float delta)
	{
		int n = 0;
		for (TimedFunc& f : functions) { // Will this cause problems? Removing an element in a for loop
			f.time -= delta;
			if (f.time < 0) {
				f.func();
				functions.erase(functions.begin() + n);
				n--;
			}
			n++;
		}
	}
	void RenderEngine(double lag)
	{
		SwitchBlendDepth(false);
		//if (CheckState(GameState::Game)) {
			if (HasFocus()) {
				//move += 0.01f;
				glm::mat4 lightView = glm::lookAt({ -2,4,-1 }, glm::vec3(0), { 0,1,0 });

				glViewport(0, 0, Width(), Height());
				glClearColor(0.05f, 0.08f, 0.08f, 1);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glCullFace(GL_BACK);

				// All this should be automated and customizable in the engine.
				// Shadow stuff
				/*
				Shader* depth = GetAsset<Shader>("depth");
				if (depth != nullptr) {
					depth->Bind();
					glCullFace(GL_BACK);
					depth->SetMatrix("uLightMatrix", GetLightProj() * lightView);
					glViewport(0, 0, 1024, 1024);
					GetDepthBuffer().Bind();
					glClear(GL_DEPTH_BUFFER_BIT);
					RenderRawObjects(depth, lag);
					GetDepthBuffer().Unbind();
				}
				*/
				Shader* objectShader = GetAsset<Shader>("object");
				if (objectShader != nullptr) {
					glViewport(0, 0, Width(), Height());
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glCullFace(GL_BACK);
					objectShader->Bind();
					objectShader->SetInt("shadow_map", 0);
					objectShader->SetMatrix("uLightSpaceMatrix", GetLightProj() * lightView);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);

					RenderObjects(lag);

					objectShader->Bind();
					MeshAsset* asset = GetAsset<MeshAsset>("Axis/Cube.031");

					for (int j = 0; j < asset->materials.size() && j < 4; j++)// Maximum of 4 materials
					{
						PassMaterial(objectShader, j, asset->materials[j]);
					}
					objectShader->SetMatrix("uTransform", glm::mat4(1));
					asset->buffer.Draw();

					//objectShader->SetMatrix("uTransform", glm::mat4(1));
					//testBuffer.Draw();
				}/*
				if (IsKeyDown(GLFW_KEY_K)) {
					//std::cout << "yes\n";
					SwitchBlendDepth(true);
					Shader* experiment = GetAsset<Shader>("experiment");
					if (experiment != nullptr) {
						experiment->Bind();
						experiment->SetInt("uTexture", 0);
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);
						cont.Draw();
					}
				}*/
			}
		//}

		RenderUI();

		RenderDebug(lag);

		console::RenderConsole();

		// Camera location
		Shader* gui = GetAsset<Shader>("gui");
		gui->Bind();
		gui->SetVec2("uPos", { 0,0 });
		gui->SetVec2("uSize", { 1,1 });
		gui->SetVec4("uColor", 1, 1, 1, 1);
		gui->SetInt("uTextured", 1);
		DrawString(GetAsset<Font>("consolas"), std::to_string(GetCamera()->position.x) + " " + std::to_string(GetCamera()->position.y) + " " + std::to_string(GetCamera()->position.z), false, 50, 300, 50, -1);
	}
	void UpdateEngine(double delta)
	{
		RefreshEvents();
		//if (CheckState(GameState::Game)) {
			//if (HasFocus() && !CheckState(GameState::Paused)) {
		UpdateObjects(delta);
			//}
		//}
		UpdateUI(delta);

		UpdateDebug(delta);

		ResetEvents();
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