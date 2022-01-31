#include "gonpch.h"

#include "Engone.h"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

namespace engone {

	void Engone::Init() {
		engone::SetState(GameState::RenderGame, true);
		engone::SetState(GameState::RenderGui, true);

		InitRenderer();

		AddAsset<Shader>("object", new Shader(objectSource, true));
		AddAsset<Shader>("armature", new Shader(armatureSource, true));
		AddAsset<Shader>("outline", new Shader(outlineSource, true));
		AddAsset<MaterialAsset>("defaultMaterial", new MaterialAsset());

		//DebugInit();
		InitEvents(GetWindow());
		InitEvents();

		InitGUI();
		//ReadOptions();

		//console::Init(120,50);

		AddListener(new Listener(EventType::Move, 9998, FirstPerson));
		AddListener(new Listener(EventType::Resize, 9999, DrawOnResize));

		//std::cout << "Using " << glGetString(GL_RENDERER) << std::endl;

		float vMap[]{ 0,0,0,1,0,0,1,0,1,0,0,1 };
		unsigned int iMap[]{ 0,1,2,2,3,0 };

		temp_.Init(true, vMap, 4 * 3, iMap, 6);
		temp_.SetAttrib(0, 3, 3, 0);

		hitbox.Init(true, nullptr, 6 * vecLimit, nullptr, 2 * lineLimit);
		hitbox.SetAttrib(0, 3, 6, 0);
		hitbox.SetAttrib(1, 3, 6, 3);
		hitboxVec = new float[vecLimit * 6];// {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1}; not using color
		hitboxInd = new unsigned int[lineLimit * 2];// {0, 1, 0, 2, 0, 3};

		//UpdateKeyboard(false);

		depthBuffer.Init();
		float near_plane = 1.f, far_plane = 20.5f;
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);

		fov = 90.f;
		zNear = 0.1f;
		zFar = 400.f;
		SetProjection((float)Width() / Height());
	}
	void Engone::Update(float delta) {
	
	}
	void Engone::Render() {
	
	}

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
	int vecLimit = 500;
	int lineLimit = 3000;
	LineBuffer hitbox;
	float* hitboxVec;
	unsigned int* hitboxInd;
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

	std::function<void(double)> update=nullptr;
	std::function<void(double)> render=nullptr;

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
	EventType DrawOnResize(Event& e) {
		//glViewport(0, 0, Width(), Height());
		//glClearColor(1, 0, 0, 1);
		//glClear(GL_COLOR_BUFFER_BIT);

		UpdateEngine(1/40.f);
		update(1 / 40.f);
		RenderEngine(0);
		render(0);

		glfwSwapBuffers(GetWindow());

		return EventType::None;
	}

	void InitEngine()
	{
		engone::SetState(GameState::RenderGame, true);
		engone::SetState(GameState::RenderGui, true);

		InitRenderer();

		AddAsset<Shader>("object", new Shader(objectSource, true));
		AddAsset<Shader>("armature", new Shader(armatureSource, true));
		AddAsset<Shader>("outline", new Shader(outlineSource, true));
		AddAsset<MaterialAsset>("defaultMaterial", new MaterialAsset());

		//DebugInit();
		InitEvents(GetWindow());
		InitEvents();

		InitGUI();
		//ReadOptions();

		//console::Init(120,50);

		AddListener(new Listener(EventType::Move,9998, FirstPerson));
		AddListener(new Listener(EventType::Resize, 9999,DrawOnResize));

		//std::cout << "Using " << glGetString(GL_RENDERER) << std::endl;

		float vMap[]{0,0,0,1,0,0,1,0,1,0,0,1};
		unsigned int iMap[]{0,1,2,2,3,0};

		temp_.Init(true,vMap,4*3,iMap,6);
		temp_.SetAttrib(0, 3, 3, 0);

		hitbox.Init(true, nullptr, 6 * vecLimit, nullptr, 2 * lineLimit);
		hitbox.SetAttrib(0, 3, 6, 0);
		hitbox.SetAttrib(1, 3, 6, 3);
		hitboxVec = new float[vecLimit * 6];// {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1}; not using color
		hitboxInd = new unsigned int[lineLimit * 2];// {0, 1, 0, 2, 0, 3};

		//UpdateKeyboard(false);

		depthBuffer.Init();
		float near_plane = 1.f, far_plane = 20.5f;
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);

		fov = 90.f;
		zNear = 0.1f;
		zFar = 400.f;
		SetProjection((float)Width() / Height());
	}
	void ExitEngine()
	{

	}

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
			o0->renderComponent.animator.Update(delta);
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
		UpdateElements(delta);
	}
	// If hitbox buffer is too small
	bool reachedLimit = false;
	void RenderRawObjects(Shader* shader, double lag) {
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
#define IFBLOCK {} if(false)
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

		//log::out << "render\n";
		SwitchBlendDepth(false);
		UpdateViewMatrix(lag);

		Shader* shader = GetAsset<Shader>("object");

		if (!shader->error)
		{
			shader->Bind();
			UpdateProjection(shader);
			shader->SetVec3("uCamera", GetCamera()->position + GetCamera()->velocity * (float)lag);
			for (GameObject* o : GetObjects())
			{
				if (o->renderComponent.model != nullptr) 
				{
					//log::out << o->name << "\n";
					BindLights(shader,o->position);
					
					// Get individual transforms
					std::vector<glm::mat4> mats;
					o->renderComponent.GetParentTransforms(mats);
					
					// Draw instances
					for (int i = 0; i < o->renderComponent.model->instances.size(); i++) 
					{
						AssetInstance& instance = o->renderComponent.model->instances[i];
						//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
						if (instance.asset->type == AssetType::Mesh) {
							MeshAsset* asset = instance.asset->cast<MeshAsset>();
							if (asset->meshType == MeshAsset::MeshType::Normal) {
								for (int j = 0; j < asset->materials.size() && j < 4; j++)// Maximum of 4 materials
								{
									PassMaterial(shader, j, asset->materials[j]);
								}
								shader->SetMatrix("uTransform", o->renderComponent.matrix * mats[i] * instance.localMat);
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
				if (o->renderComponent.model != nullptr)
				{
					BindLights(shader,o->position);

					// Get individual transforms
					std::vector<glm::mat4> mats;
					o->renderComponent.GetParentTransforms(mats);

					//-- Draw instances
					for (int i = 0; i < o->renderComponent.model->instances.size(); i++) 
					{
						AssetInstance& mInst= o->renderComponent.model->instances[i];

						if (mInst.asset->type == AssetType::Mesh) {
							MeshAsset* meshAsset = mInst.asset->cast<MeshAsset>();
							if (meshAsset->meshType == MeshAsset::MeshType::Boned) {
								if (mInst.parent == -1)
									continue;
								
								AssetInstance& aInst = o->renderComponent.model->instances[mInst.parent];
								if (aInst.asset->type == AssetType::Armature) {
									ArmatureAsset* armatureAsset = aInst.asset->cast<ArmatureAsset>();

									std::vector<glm::mat4> boneMats;
									o->renderComponent.GetArmatureTransforms(boneMats, mats[i], aInst, armatureAsset);
									for (int j = 0; j < boneMats.size(); j++)
									{
										shader->SetMatrix("uBoneTransforms[" + std::to_string(j) + "]", boneMats[j] * mInst.localMat);
									}

									for (int j = 0; j < meshAsset->materials.size() && j < 4; j++)// Maximum of 4 materials
									{
										PassMaterial(shader, j, meshAsset->materials[j]);
									}
									shader->SetMatrix("uTransform", o->renderComponent.matrix*mats[i]);
									meshAsset->buffer.Draw();
								}
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
		RenderElements();
	}
	static std::vector<Timer> timers;
	void AddTimer(float time, std::function<void()> func)
	{
		timers.push_back({time,func});
	}
	void UpdateTimers(float delta)
	{
		for (int i = 0; i < timers.size();i++) { // Will this cause problems? Removing an element in a for loop
			timers[i].time -= delta;
			if (timers[i].time < 0) {
				timers[i].func();
				timers.erase(timers.begin() + i);
				i--;
			}
		}
	}
	void RenderEngine(double lag)
	{
		glViewport(0, 0, Width(), Height());

		SwitchBlendDepth(false);
		if (CheckState(GameState::RenderGame)) {
			if (HasFocus()) {
				//move += 0.01f;
				glm::mat4 lightView = glm::lookAt({ -2,4,-1 }, glm::vec3(0), { 0,1,0 });

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
					objectShader->SetMatrix("uTransform", glm::translate(glm::mat4(1),glm::vec3(0,-5,0)));
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
		}
		if (CheckState(GameState::RenderGui)) {
			RenderUI(0);
		}

		//RenderDebug(lag);

		//console::Render();

		//-- Camera location
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
		if (CheckState(GameState::RenderGame)) {
			//if (HasFocus() && !CheckState(GameState::Paused)) {
			UpdateObjects(delta);
			//}
		}
		UpdateTimers(delta);
		if (CheckState(GameState::RenderGui)) {
			UpdateUI(delta);
		}

		UpdateDebug(delta);

		ResetEvents();
	}

	static std::vector<Light*> lights;
	void AddLight(Light* l)
	{
		lights.push_back(l);
	}
	void RemoveLight(Light* l)
	{
		for (int i = 0; i < lights.size(); i++) {
			if (lights[i] == l) {
				lights.erase(lights.begin() + i);
				break;
			}
		}
	}
	/*
	Binds light to current shader
	 If one of the four closest light are already bound then don't rebind them [Not added]
	*/
	void BindLights(Shader* shader, glm::vec3 objectPos)
	{
		if (shader != nullptr) {
			// List setup
			const int N_POINTLIGHTS = 4;
			const int N_SPOTLIGHTS = 2;

			DirLight* dir = nullptr;
			PointLight* points[N_POINTLIGHTS];
			float pDist[N_POINTLIGHTS];
			for (int i = 0; i < N_POINTLIGHTS; i++) {
				points[i] = nullptr;
				pDist[i] = 9999;
			}

			SpotLight* spots[N_SPOTLIGHTS];
			float sDist[N_SPOTLIGHTS];
			for (int i = 0; i < N_SPOTLIGHTS; i++) {
				spots[i] = nullptr;
				sDist[i] = 9999;
			}
			glm::ivec3 lightCount(0);

			// Grab the closest lights to the object
			for (int i = 0; i < lights.size(); i++) {
				Light* light = lights[i];
				if (light->lightType == LightType::Direction) {
					dir = reinterpret_cast<DirLight*>(light);
				}
				else if (light->lightType == LightType::Point) {
					PointLight* l = reinterpret_cast<PointLight*>(light);
					float distance = glm::length(l->position - objectPos);
					for (int i = 0; i < N_POINTLIGHTS; i++) {
						if (points[i] == nullptr) {
							points[i] = l;
							pDist[i] = distance;
							break;
						}
						else {
							if (distance < pDist[i]) {
								PointLight* pTemp = points[i];
								float dTemp = pDist[i];
								points[i] = l;
								pDist[i] = distance;
							}
						}
					}
				}
				else if (light->lightType == LightType::Spot) {
					SpotLight* l = reinterpret_cast<SpotLight*>(light);
					float distance = glm::length(l->position - objectPos);
					for (int i = 0; i < N_SPOTLIGHTS; i++) {
						if (spots[i] == nullptr) {
							spots[i] = l;
							sDist[i] = distance;
							break;
						}
						else {
							if (distance < sDist[i]) {
								SpotLight* lTemp = spots[i];
								float dTemp = sDist[i];
								spots[i] = l;
								sDist[i] = distance;
							}
						}
					}
				}
			}

			// Pass light to shader
			if (dir != nullptr) {
				PassLight(shader, dir);
				lightCount.x++;
			}

			for (int i = 0; i < N_POINTLIGHTS; i++) {
				if (points[i] != nullptr) {
					PassLight(shader, i, points[i]);
					lightCount.y++;
				}
				else
					break;
			}
			for (int i = 0; i < N_SPOTLIGHTS; i++) {
				if (spots[i] != nullptr) {
					PassLight(shader, i, spots[i]);
					lightCount.z++;
				}
				else
					break;
			}
			shader->SetIVec3("uLightCount", lightCount);
		}
	}
	std::vector<Light*>& GetLights()
	{
		return lights;
	}
	void PassLight(Shader* shader, DirLight* light)
	{
		if (light != nullptr) {
			shader->SetVec3("uDirLight.ambient", light->ambient);
			shader->SetVec3("uDirLight.diffuse", light->diffuse);
			shader->SetVec3("uDirLight.specular", light->specular);
			shader->SetVec3("uDirLight.direction", light->direction);
		}
	}
	void PassLight(Shader* shader, int index, PointLight* light)
	{
		std::string u = "uPointLights[" + index + (std::string)"].";
		if (light != nullptr) {
			shader->SetVec3(u + "ambient", light->ambient);
			shader->SetVec3(u + "diffuse", light->diffuse);
			shader->SetVec3(u + "specular", light->specular);
			shader->SetVec3(u + "position", light->position);
			shader->SetFloat(u + "constant", light->constant);
			shader->SetFloat(u + "linear", light->linear);
			shader->SetFloat(u + "quadratic", light->quadratic);
		}
	}
	void PassLight(Shader* shader, int index, SpotLight* light)
	{
		std::string u = "uSpotLights[" + index + (std::string)"].";
		if (light != nullptr) {
			shader->SetVec3(u + "ambient", light->ambient);
			shader->SetVec3(u + "diffuse", light->diffuse);
			shader->SetVec3(u + "specular", light->specular);
			shader->SetVec3(u + "position", light->position);
			shader->SetVec3(u + "direction", light->direction);
			shader->SetFloat(u + "cutOff", light->cutOff);
			shader->SetFloat(u + "outerCutOff", light->outerCutOff);
		}
	}
	void PassMaterial(Shader* shader, int index, MaterialAsset* material)
	{
		if (shader != nullptr && material != nullptr) {
			if (material->diffuse_map != nullptr) {

				material->diffuse_map->Bind(index + 1);
				//BindTexture(index + 1, material->diffuse_map);// + 1 because of shadow_map on 0
				//std::cout << "PassMaterial - texture not bound!\n";
				shader->SetInt("uMaterials[" + std::to_string(index) + "].diffuse_map", index + 1);
				shader->SetInt("uMaterials[" + std::to_string(index) + "].useMap", 1);
			}
			else {
				shader->SetInt("uMaterials[" + std::to_string(index) + "].useMap", 0);
			}
			shader->SetVec3("uMaterials[" + std::to_string(index) + "].diffuse_color", material->diffuse_color);
			shader->SetVec3("uMaterials[" + std::to_string(index) + "].specular", material->specular);
			shader->SetFloat("uMaterials[" + std::to_string(index) + "].shininess", material->shininess);
		}
		else {
			//std::cout << "shader or material is nullptr in Passmaterial\n";
		}
	}
	void Start(std::function<void(double)> inUpdate, std::function<void(double)> inRender,double fps){
		if (inUpdate == nullptr || inRender == nullptr) {
			std::cout << "Engone.cpp:Start() - update or render is nullptr" << "\n";
			return;
		}
		update = inUpdate;
		render = inRender;

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

		double previous = GetAppTime();
		double lag = 0.0;
		double MS_PER_UPDATE = 1. / fps;
		double lastSecond=previous;
		int FPS = 0;
		while (RenderRunning()) {
			double current = GetAppTime();
			double elapsed = current - previous;
			//std::cout << elapsed << std::endl;
			previous = current;
			lag += elapsed;

			while (lag >= MS_PER_UPDATE)
			{
				UpdateEngine(MS_PER_UPDATE);
				update(MS_PER_UPDATE);
				lag -= MS_PER_UPDATE;
				FPS++;
				if (current-lastSecond>1) {
					//std::cout << (current - lastSecond) << std::endl;
					lastSecond = current;
					//SetWindowTitle(("Project Unknown  " + std::to_string(FPS) + " fps").c_str());
					FPS = 0;
				}
			}
			RenderEngine(lag);
			render(lag);

			glfwSwapBuffers(GetWindow());
			glfwPollEvents();
		}
		// remove buffer and stuff or not?
		glfwTerminate();
	}
}