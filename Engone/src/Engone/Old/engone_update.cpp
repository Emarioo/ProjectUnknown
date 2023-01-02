/*
	This code is basically useless because ReactPhysics3D replaced the entity component system I had.
	But... i don't want to delete the code so here it is.
*/

// ################################ HEADER

//void Initialize(EngoneOption hints=EngoneOption::None);

	/*
	Start the loop running the applications.
	When all windows of all applications are closed the loop will end.
	*/
	//void Start();
	//void AddApplication(Application* app);

	//FrameBuffer& GetDepthBuffer();

	//Camera* GetCamera();

	/*
	Update objects and camera (viewMatrix)
	 also dimensions but they should be moved somewhere else.
	*/
	//void UpdateObjects(float delta);

	/*
	Render objects, hitboxes with the appropriate shaders.
	*/
	//void RenderObjects(float lag);
	/*
	Render objects without doing anything but calling the object's draw function.
	*/
	//void RenderRawObjects(Shader* shader, float lag);

	/*
	Render objects, ui, debug tool
	*/
	//void RenderEngine(float lag);
	/*
	Update objects, ui, debug tool
	*/
	//void UpdateEngine(float delta);

	//struct Delayed {
	//	float time;
	//	std::function<void()> func;
	//};

	//void AddTimer(float time, std::function<void()> func);
	//void UpdateTimers(float delta);

	// Light
	//void AddLight(Light* l);
	//void RemoveLight(Light* l);

	/*
	Binds light to current shader
	 If one of the four closest light are already bound then don't rebind them [Not added]
	*/
	//void BindLights(Shader* shader, glm::vec3 objectPos);
	//std::vector<Light*>& GetLights();

// ################################################################################# SOURCE

//std::vector<Light*>& GetLights() {
//	return lights;
//}
//void UpdateObjects(float delta) {
	//std::vector<Collider> colliders;

	//// Spatial partitioning?
	//
	//EntityIterator iterator = GetEntityIterator(Transform::ID | Physics::ID);
	//while (iterator) {
	//	Transform* transform = iterator.get<Transform>();
	//	Physics* physics = iterator.get<Physics>();
	//	physics->lastPosition = transform->position;
	//}
	//
	//RunUpdateSystems(delta);
	//
	////-- Movement and physics
	////EntityIterator iterator = GetEntityIterator(ComponentEnum::Transform|ComponentEnum::Physics);
	//while (iterator) {
	//	Transform* transform = iterator.get<Transform>();
	//	Physics* physics = iterator.get<Physics>();
	//	//ModelRenderer* renderer = iterator.get<ModelRenderer>();
	//	//Scriptable* scriptable = iterator.get<Scriptable>();
	//	/*
	//	if (scriptable)
	//		scriptable->entity->Update(delta);
	//	*/
	//	if (physics->movable) {
	//		physics->velocity.y += physics->gravity * delta;
	//		transform->position += physics->velocity * delta;
	//	}
	//}
	//iterator = GetEntityIterator(Transform::ID | Physics::ID | Collision::ID);
	//while (iterator) {
	//	Transform* transform = iterator.get<Transform>();
	//	Physics* physics = iterator.get<Physics>();
	//	Collision* collision = iterator.get<Collision>();

	//	ColliderAsset* asset = collision->asset;

	//	glm::mat4 matrix = glm::translate(transform->position);

	//	if (asset->colliderType == ColliderAsset::Type::Cube) {
	//		matrix = glm::scale(matrix, asset->cube.scale);
	//	}

	//	colliders.push_back({ asset, iterator.getEntity() });
	//	glm::vec3 skew;
	//	glm::quat rotation;
	//	glm::vec4 persp;
	//	glm::decompose(matrix, colliders.back().scale, rotation, colliders.back().position, skew, persp);

	//	if (asset->colliderType == ColliderAsset::Type::Sphere) {
	//		colliders.back().position += asset->sphere.position;
	//	} else if (asset->colliderType == ColliderAsset::Type::Cube) {
	//		colliders.back().position += asset->cube.position;
	//	}
	//}
	//iterator = GetEntityIterator(Transform::ID | Physics::ID | ModelRenderer::ID);
	//while(iterator){
	//	Transform* transform = iterator.get<Transform>();
	//	Physics* physics = iterator.get<Physics>();
	//	ModelRenderer* renderer = iterator.get<ModelRenderer>();

	//	if (renderer->asset) {
	//		ModelAsset* model = renderer->asset;

	//		glm::mat4 modelMatrix = glm::translate(transform->position)
	//			* glm::rotate(transform->rotation.x, glm::vec3(1, 0, 0))
	//			* glm::rotate(transform->rotation.z, glm::vec3(0, 0, 1))
	//			* glm::rotate(transform->rotation.y, glm::vec3(0, 1, 0))
	//			* glm::scale(transform->scale);

	//		// Get individual transforms
	//		std::vector<glm::mat4> transforms;
	//		Animator* animator = iterator.get<Animator>();
	//		if(animator)
	//			model->getParentTransforms(animator, transforms);

	//		for (uint32_t j = 0; j < model->instances.size(); ++j) {
	//			AssetInstance& instance = model->instances[j];

	//			if (instance.asset->type == AssetType::Collider) {
	//				ColliderAsset* asset = instance.asset->cast<ColliderAsset>();

	//				glm::mat4 matrix;
	//				if(animator)
	//					matrix = modelMatrix * transforms[j] * instance.localMat;
	//				else
	//					matrix = modelMatrix * instance.localMat;

	//				/*if (asset->colliderType==ColliderAsset::Type::Cube) {
	//					matrix = glm::scale(matrix, asset->cube.scale);
	//				}*/

	//				if (asset->colliderType == ColliderAsset::Type::Sphere) {
	//					matrix *= glm::translate(asset->sphere.position);
	//					//colliders.back().position += asset->sphere.position;
	//				} else if (asset->colliderType == ColliderAsset::Type::Cube) {
	//					matrix *= glm::translate(asset->cube.position);
	//					//colliders.back().position += asset->cube.position;
	//				}


	//				colliders.push_back({ asset, iterator.getEntity() });
	//				colliders.back().matrix = matrix;

	//				glm::vec3 position;
	//				glm::vec3 scale;
	//				glm::vec3 skew;
	//				glm::quat rotation;
	//				glm::vec4 persp;
	//				glm::decompose(matrix, scale, rotation, position, skew, persp);

	//				//PlaneCollider col = MakeCubeCollider(matrix, scale);

	//				colliders.back().position = position;
	//				colliders.back().scale = scale;

	//				/*if (asset->colliderType == ColliderAsset::Type::Sphere) {
	//					colliders.back().position += asset->sphere.position;
	//				} else if (asset->colliderType == ColliderAsset::Type::Cube) {
	//					colliders.back().position += asset->cube.position;
	//				}*/
	//			}
	//		}
	//	}

	//}

	////-- Collision
	//while (colliders.size()>1) {
	//	Collider& c1 = colliders[0];
	//	for (uint32_t i = 1; i < colliders.size();++i) {
	//		Collider& c2 = colliders[i];

	//		if (glm::length(c1.position - c2.position) < c1.asset->furthest+c2.asset->furthest) {
	//			
	//			bool yes = TestComplexCollision(&c1,&c2);
	//			if (yes) {
	//				RunCollisionSystems(c1, c2);
	//			}
	//		}
	//	}
	//	// remove collider when done with calculations
	//	colliders.erase(colliders.begin());
	//}
//}
//void RenderRawObjects(Shader* shader, float lag) {
//	//for (GameObject* o : GetObjects()) {
//		/*
//		if (o->renderComponent.model != nullptr) {
//			for (int i = 0; i < o->renderComponent.model->meshes.size(); i++) {
//				//std::cout << " okay2\n";
//				Mesh* mesh = o->renderComponent.model->meshes[i];
//				if (mesh != nullptr) {
//					//std::cout << o->GetName()<<" okay3\n";
//					shader->SetMatrix("uTransform", o->renderComponent.matrix);
//					/*  *
//					glm::translate(o->velocity * (float)lag)* // special
//						o->renderComponent.model->matrices[i]
//					/*
//					SetTransform(o->renderComponent.matrix*
//						glm::translate(o->velocity*(float)lag)* // special
//						o->renderComponent.model->matrices[i]);
//					
//					mesh->Draw();
//				}
//			}
//		}
//		*/
//	//}
//}
//void RenderObjects(float lag) {
	//// Apply lag transformation on objects?

	//EnableDepth();
	//UpdateViewMatrix(lag);

	//std::unordered_map<MeshAsset*, std::vector<glm::mat4>> normalObjects;
	////std::unordered_map<MeshAsset*, std::vector<glm::mat4>> bonedObjects; Not supported

	//EntityIterator meshIterator = GetEntityIterator(Transform::ID | MeshRenderer::ID);
	//while (meshIterator) {
	//	MeshRenderer* renderer = meshIterator.get<MeshRenderer>();
	//	if (renderer->asset && renderer->visible) {
	//		Transform* transform = meshIterator.get<Transform>();
	//		glm::mat4 modelMatrix = glm::translate(transform->position)
	//			* glm::rotate(transform->rotation.x, glm::vec3(1, 0, 0))
	//			* glm::rotate(transform->rotation.z, glm::vec3(0, 0, 1))
	//			* glm::rotate(transform->rotation.y, glm::vec3(0, 1, 0))
	//			* glm::scale(transform->scale);

	//		MeshAsset* asset = renderer->asset;

	//		if (asset->meshType == MeshAsset::MeshType::Normal) {
	//			if (normalObjects.count(asset) > 0) {
	//				normalObjects[asset].push_back(modelMatrix);
	//			} else {
	//				normalObjects[asset] = std::vector<glm::mat4>();
	//				normalObjects[asset].push_back(modelMatrix);
	//			}
	//		}
	//	}
	//}
	//EntityIterator modelIterator = GetEntityIterator(Transform::ID | ModelRenderer::ID);
	//while (modelIterator) {
	//	ModelRenderer* renderer = modelIterator.get<ModelRenderer>();

	//	if (renderer->asset && renderer->visible) {
	//		ModelAsset* model = renderer->asset;

	//		Transform* transform = modelIterator.get<Transform>();
	//		glm::mat4 modelMatrix = glm::translate(transform->position)
	//			* glm::rotate(transform->rotation.x, glm::vec3(1, 0, 0))
	//			* glm::rotate(transform->rotation.z, glm::vec3(0, 0, 1))
	//			* glm::rotate(transform->rotation.y, glm::vec3(0, 1, 0))
	//			* glm::scale(transform->scale);

	//		Animator* animator = modelIterator.get<Animator>();

	//		// Get individual transforms
	//		std::vector<glm::mat4> transforms;
	//		if (animator)
	//			model->getParentTransforms(animator, transforms);

	//		/*
	//		if ((transform->position.x-camPos.x)* (transform->position.x - camPos.x)
	//			+(transform->position.y - camPos.y)*(transform->position.y - camPos.y)+
	//			(transform->position.z - camPos.z)*(transform->position.z - camPos.z)>200*200) {
	//			continue;
	//		}
	//		*/

	//		// Draw instances
	//		for (uint32_t i = 0; i < model->instances.size(); ++i) {
	//			AssetInstance& instance = model->instances[i];
	//			//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
	//			if (instance.asset->type == AssetType::Mesh) {
	//				MeshAsset* asset = instance.asset->cast<MeshAsset>();

	//				if (asset->meshType == MeshAsset::MeshType::Normal) {
	//					glm::mat4 out;

	//					if (animator)
	//						out = modelMatrix * transforms[i] * instance.localMat;
	//					else
	//						out = modelMatrix * instance.localMat;

	//					if (normalObjects.count(asset) > 0) {
	//						normalObjects[asset].push_back(out);
	//					} else {
	//						normalObjects[asset] = std::vector<glm::mat4>();
	//						normalObjects[asset].push_back(out);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//Shader* shader = GetAsset<Shader>("object");
	//if (!shader->error) {
	//	shader->bind();
	//	UpdateProjection(shader);
	//	shader->setVec3("uCamera", GetCamera()->position + GetCamera()->velocity * (float)lag);
	//	glm::vec3 camPos = GetCamera()->position;

	//	BindLights(shader, { 0,0,0 });
	//	shader->setMat4("uTransform", glm::mat4(0));
	//	for (auto& [asset, vector] : normalObjects) {
	//		//if (asset->meshType == MeshAsset::MeshType::Normal) {
	//			for (uint32_t j = 0; j < asset->materials.size(); ++j) {// Maximum of 4 materials
	//				asset->materials[j]->bind(shader, j);
	//			}
	//		//}
	//			uint32_t renderAmount = 0;
	//		while (renderAmount<vector.size()) {
	//			uint32_t amount = vector.size()-renderAmount;
	//			if (amount>INSTANCE_LIMIT) {
	//				amount = INSTANCE_LIMIT;
	//			}
	//			instanceBuffer.setData(amount * 16, (float*)(vector.data()+renderAmount));
	//			
	//			asset->vertexArray.selectBuffer(3, &instanceBuffer);

	//			asset->vertexArray.draw(&asset->indexBuffer, amount);
	//			renderAmount += amount;
	//		}
	//	}
	//	/*
	//	EntityIterator iterator = GetEntityIterator(Transform::ID |ModelRenderer::ID);
	//	while (false) {
	//		ModelRenderer* renderer = iterator.get<ModelRenderer>();
	//		
	//		if (renderer->asset&&renderer->visible) {
	//			ModelAsset* model = renderer->asset;
	//			
	//			Transform* transform = iterator.get<Transform>();
	//			Animator* animator = iterator.get<Animator>();
	//			
	//			BindLights(shader, transform->position);

	//			// Get individual transforms
	//			std::vector<glm::mat4> transforms;
	//			if (animator)
	//				model->GetParentTransforms(animator, transforms);
	//			
	//			if ((transform->position.x-camPos.x)* (transform->position.x - camPos.x)
	//				+(transform->position.y - camPos.y)*(transform->position.y - camPos.y)+
	//				(transform->position.z - camPos.z)*(transform->position.z - camPos.z)>200*200) {
	//				continue;
	//			}

	//			// Draw instances
	//			for (int i = 0; i < model->instances.size(); i++) {
	//				AssetInstance& instance = model->instances[i];
	//				//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
	//				if (instance.asset->type == AssetType::Mesh) {
	//					MeshAsset* asset = instance.asset->cast<MeshAsset>();
	//					if (asset->meshType == MeshAsset::MeshType::Normal) {
	//						for (int j = 0; j < asset->materials.size() && j < 4; j++)// Maximum of 4 materials
	//						{
	//							asset->materials[j]->Bind(shader, j);
	//						}
	//						if(animator)
	//							shader->SetMatrix("uTransform", glm::translate(transform->position) * transforms[i] * instance.localMat);
	//						else
	//							shader->SetMatrix("uTransform", glm::translate(transform->position) * instance.localMat);
	//						asset->vertexArray.draw(&asset->indexBuffer);
	//					}
	//				}
	//			}
	//		}
	//	}
	//	*/
	//}

	//shader = GetAsset<Shader>("armature");
	//if (!shader->error) {
	//	shader->bind();
	//	UpdateProjection(shader);
	//	shader->setVec3("uCamera", GetCamera()->position + GetCamera()->velocity * (float)lag);

	//	//EntityIterator iterator = GetEntityIterator(ComponentEnum::Transform | ComponentEnum::ModelRenderer);
	//	while (modelIterator) {
	//		ModelRenderer* renderer = modelIterator.get<ModelRenderer>();

	//		if (renderer->asset && renderer->visible) {
	//			ModelAsset* model = renderer->asset;
	//		
	//			Transform* transform = modelIterator.get<Transform>();
	//			glm::mat4 modelMatrix = glm::translate(glm::mat4(1), transform->position)
	//				* glm::rotate(transform->rotation.x, glm::vec3(1, 0, 0))
	//				* glm::rotate(transform->rotation.z, glm::vec3(0, 0, 1))
	//				* glm::rotate(transform->rotation.y, glm::vec3(0, 1, 0))
	//				* glm::scale(transform->scale);

	//			Animator* animator = modelIterator.get<Animator>();

	//			BindLights(shader, transform->position);

	//			// Get individual transforms
	//			std::vector<glm::mat4> transforms;
	//			if (animator)
	//				model->getParentTransforms(animator, transforms);

	//			//-- Draw instances
	//			AssetInstance* armatureInstance=nullptr;
	//			ArmatureAsset* armatureAsset=nullptr;
	//			for (uint32_t i = 0; i < model->instances.size(); ++i) {
	//				AssetInstance& instance = model->instances[i];

	//				if (instance.asset->type == AssetType::Armature) {
	//					armatureInstance = &instance;
	//					armatureAsset = instance.asset->cast<ArmatureAsset>();
	//				} else if (instance.asset->type == AssetType::Mesh) {
	//					if (!armatureInstance)
	//						continue;

	//					MeshAsset* meshAsset = instance.asset->cast<MeshAsset>();
	//					if (meshAsset->meshType == MeshAsset::MeshType::Boned) {
	//						if (instance.parent == -1)
	//							continue;

	//						/*if (animator->enabledAnimations[0].asset) {
	//							log::out << animator->enabledAnimations->frame<<"\n";
	//						}*/

	//						std::vector<glm::mat4> boneTransforms;
	//						if (animator)
	//							model->getArmatureTransforms(animator, boneTransforms, transforms[i], armatureInstance, armatureAsset);

	//						for (uint32_t j = 0; j < boneTransforms.size(); ++j) {
	//							shader->setMat4("uBoneTransforms[" + std::to_string(j) + "]", boneTransforms[j] * instance.localMat);
	//						}

	//						for (uint32_t j = 0; j < meshAsset->materials.size(); ++j){// Maximum of 4 materials
	//							meshAsset->materials[j]->bind(shader, j);
	//						}
	//						if (animator) {
	//							shader->setMat4("uTransform", modelMatrix * transforms[i]);
	//						} else {
	//							shader->setMat4("uTransform", modelMatrix);
	//						}
	//						meshAsset->vertexArray.draw(&meshAsset->indexBuffer);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
	//shader = GetAsset<Shader>("collision");
	//if (shader != nullptr) {
	//	shader->bind();
	//	UpdateProjection(shader);
	//	glLineWidth(1.f);
	//	shader->setVec3("uColor", { 0.05,0.9,0.1 });

	//	EntityIterator modelIterator = GetEntityIterator(ComponentEnum::Transform | ComponentEnum::ModelRenderer | ComponentEnum::Physics);
	//	while (modelIterator) {
	//		ModelRenderer* renderer = modelIterator.get<ModelRenderer>();
	//		Physics* physics = modelIterator.get<Physics>();

	//		if (renderer->asset && physics->renderCollision) {
	//			ModelAsset* model = renderer->asset;

	//			Transform* transform = modelIterator.get<Transform>();
	//			glm::mat4 modelMatrix = glm::translate(glm::mat4(1), transform->position)
	//				* glm::rotate(transform->rotation.x, glm::vec3(1, 0, 0))
	//				* glm::rotate(transform->rotation.z, glm::vec3(0, 0, 1))
	//				* glm::rotate(transform->rotation.y, glm::vec3(0, 1, 0))
	//				* glm::scale(transform->scale);

	//			Animator* animator = modelIterator.get<Animator>();

	//			// Get individual transforms
	//			std::vector<glm::mat4> transforms;
	//			if (animator)
	//				model->getParentTransforms(animator, transforms);

	//			// Draw instances
	//			for (uint32_t i = 0; i < model->instances.size(); ++i) {
	//				AssetInstance& instance = model->instances[i];
	//				//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
	//				if (instance.asset->type == AssetType::Collider) {
	//					ColliderAsset* asset = instance.asset->cast<ColliderAsset>();
	//					glm::vec3 offset = glm::vec3(0);
	//					if (asset->colliderType == ColliderAsset::Type::Sphere) {
	//						offset = asset->sphere.position;
	//					} else if (asset->colliderType == ColliderAsset::Type::Cube) {
	//						offset = asset->cube.position;
	//					}
	//					glm::mat4 out;
	//					if (animator)
	//						out = modelMatrix * glm::translate(offset) * transforms[i] * instance.localMat;
	//					else
	//						out = modelMatrix * glm::translate(offset) * instance.localMat;

	//					if (asset->colliderType == ColliderAsset::Type::Sphere) {
	//						//DrawSphere(asset->sphere.radius);

	//					} else if (asset->colliderType == ColliderAsset::Type::Cube) {
	//						DrawNetCube(out, asset->cube.scale);
	//						//DrawCube(asset->cube.scale.x, asset->cube.scale.y, asset->cube.scale.z);
	//					} else if (asset->colliderType == ColliderAsset::Type::Mesh) {
	//						DrawBegin();
	//						for (uint32_t i = 0; i < asset->points.size(); ++i) {
	//							AddVertex(asset->points[i].x, asset->points[i].y, asset->points[i].z);
	//						}
	//						auto& tris = asset->tris;
	//						for (uint32_t i = 0; i < asset->tris.size() / 3; ++i) {
	//							AddIndex(tris[i * 3], tris[i * 3 + 1]);
	//							AddIndex(tris[i * 3 + 1], tris[i * 3 + 2]);
	//							AddIndex(tris[i * 3 + 2], tris[i * 3]);
	//						}
	//						DrawBuffer();
	//					}
	//				}
	//			}
	//		}
	//	}

	//	//debug
	//	shader->setVec3("uColor", { 0.7,0.1,0.1 });
	//	shader->setMat4("uTransform", glm::mat4(1));

	//	DrawLines();
	//	//ClearLines();
	//}
	///*
	//Mesh* lightCube = GetMeshAsset("LightCube");
	//if (lightCube!=nullptr) {
	//	if (BindShader(ShaderLight)) {
	//		PassProjection();
	//		//bug::out < "ea" < GetLights().size()< bug::end;
	//		for (Light* l : GetLights()) {
	//			//if (l->lightType != LightType::Direction) {
	//				//bug::out < "rend light" < bug::end;
	//				GetShader(ShaderCurrent)->SetVec3("uLightColor", l->diffuse);
	//				PassTransform(glm::translate(l->position));
	//				lightCube->Draw();
	//			//}
	//		}
	//	}
	//}
	//*/
	///*
	//if (BindShader(ShaderType::Terrain)) {
	//	SetProjection();
	//	Dimension* dim = GetDimension();
	//	if (dim != nullptr) {
	//		BindTexture(0,"blank");
	//		for (Chunk c : dim->loadedChunks) {
	//			Location base;
	//			base.Translate(glm::vec3(c.x * (dim->chunkSize), 0, c.z * (dim->chunkSize)));
	//			PassTransform(base.mat());
	//			c.con.Draw();
	//		}
	//	}
	//}*/


//}
//void RenderEngine(float lag) {
//	glViewport(0, 0, (uint32_t)GetWidth(), (uint32_t)GetHeight());

//	EnableDepth();

//	//if (GetWindow()->hasFocus()) {

//		//glClearColor(0.05f, 0.08f, 0.08f, 1);
//		glClearColor(0.15f, 0.18f, 0.18f, 1);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glCullFace(GL_BACK);
//		//if (GetEngoneOptions() == EngoneOption::Game3D) {
//			// All this should be automated and customizable in the engine.
//			// Shadow stuff
//			glm::mat4 lightView = glm::lookAt({ -2,4,-1 }, glm::vec3(0), { 0,1,0 });
//			/*
//			Shader* depth = GetAsset<Shader>("depth");
//			if (depth != nullptr) {
//				depth->Bind();
//				glCullFace(GL_BACK);
//				depth->SetMatrix("uLightMatrix", GetLightProj() * lightView);
//				glViewport(0, 0, 1024, 1024);
//				GetDepthBuffer().Bind();
//				glClear(GL_DEPTH_BUFFER_BIT);
//				RenderRawObjects(depth, lag);
//				GetDepthBuffer().Unbind();
//			}
//			*/
//			Shader* objectShader = GetAsset<Shader>("object");
//			if (objectShader != nullptr) {
//				objectShader->bind();

//				objectShader->setInt("shadow_map", 0);
//				objectShader->setMat4("uLightSpaceMatrix", GetLightProj() * lightView);
//				glActiveTexture(GL_TEXTURE0);
//				glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);

//				MeshAsset* asset = GetAsset<MeshAsset>("Axis/Cube.031");

//				for (uint32_t j = 0; j < asset->materials.size(); ++j) {// Maximum of 4 materials
//					asset->materials[j]->bind(objectShader, j);
//				}
//				objectShader->setMat4("uTransform", glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
//				asset->vertexArray.draw(&asset->indexBuffer);

//				RenderObjects(lag);

//				//objectShader->SetMatrix("uTransform", glm::mat4(1));
//				//testBuffer.Draw();
//			}/*
//			if (IsKeyDown(GLFW_KEY_K)) {
//				//std::cout << "yes\n";
//				SwitchBlendDepth(true);
//				Shader* experiment = GetAsset<Shader>("experiment");
//				if (experiment != nullptr) {
//					experiment->Bind();
//					experiment->SetInt("uTexture", 0);
//					glActiveTexture(GL_TEXTURE1);
//					glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);
//					cont.Draw();
//				}
//			}*/
//			RenderRenderer();
//			//-- Debug
//			EnableBlend();
//			Shader* gui = GetAsset<Shader>("gui");
//			gui->bind();
//			gui->setVec2("uPos", { 0,0 });
//			gui->setVec2("uWindow", { GetWidth(), GetHeight() });
//			gui->setVec2("uSize", { 1,1 });
//			gui->setVec4("uColor", 1, 1, 1, 1);
//			gui->setInt("uColorMode", 1);
//			DrawString(GetAsset<Font>("consolas"), std::to_string(GetCamera()->position.x) + " " + std::to_string(GetCamera()->position.y) + " " + std::to_string(GetCamera()->position.z), false, 50, 300, 50, -1);
//			//DrawString(GetAsset<Font>("consolas"), "Chicken ma swwet sdo", false, 50, 300, 50, -1);
//		//}
//	//}

//	//if (GetEngoneOptions() == EngoneOption::UI) {
//		//RenderElements();
//		RenderUIPipeline();
//	//}
//}
//void UpdateEngine(float delta) {
//	//if (CheckState(GameState::RenderGame)) {
//		//if (HasFocus() && !CheckState(GameState::Paused)) {
//	//if (GetEngoneOptions() == EngoneOption::Game3D) {
//		UpdateObjects(delta);
//	//}
//		//}
//	//}
//	//UpdateTimers(delta);
//	//if (CheckState(GameState::RenderGui)) {
//		//UpdateElements(delta);
//	//}
//}
//static std::vector<Delayed> timers;
//void AddTimer(float time, std::function<void()> func) {
//	timers.push_back({ time,func });
//}
//void UpdateTimers(float delta) {
//	for (uint32_t i = 0; i < timers.size(); ++i) { // Will this cause problems? Removing an element in a for loop
//		timers[i].time -= delta;
//		if (timers[i].time < 0) {
//			timers[i].func();
//			timers.erase(timers.begin() + i);
//			--i;
//		}
//	}
//}

//void AddLight(Light* l)
//{
//	lights.push_back(l);
//}
//void RemoveLight(Light* l)
//{
//	for (uint32_t i = 0; i < lights.size(); ++i) {
//		if (lights[i] == l) {
//			lights.erase(lights.begin() + i);
//			break;
//		}
//	}
//}
//void BindLights(Shader* shader, glm::vec3 objectPos) {
//	if (shader != nullptr) {
//		// List setup
//		const int N_POINTLIGHTS = 4;
//		const int N_SPOTLIGHTS = 2;

//		DirLight* dir = nullptr;
//		PointLight* points[N_POINTLIGHTS];
//		float pDist[N_POINTLIGHTS];
//		for (uint32_t i = 0; i < N_POINTLIGHTS; ++i) {
//			points[i] = nullptr;
//			pDist[i] = 9999;
//		}

//		SpotLight* spots[N_SPOTLIGHTS];
//		float sDist[N_SPOTLIGHTS];
//		for (uint32_t i = 0; i < N_SPOTLIGHTS; ++i) {
//			spots[i] = nullptr;
//			sDist[i] = 9999;
//		}
//		glm::ivec3 lightCount(0);

//		// Grab the closest lights to the object
//		for (uint32_t i = 0; i < lights.size(); ++i) {
//			Light* light = lights[i];
//			if (light->lightType == LightType::Direction) {
//				dir = reinterpret_cast<DirLight*>(light);
//			} else if (light->lightType == LightType::Point) {
//				PointLight* l = reinterpret_cast<PointLight*>(light);
//				float distance = glm::length(l->position - objectPos);
//				for (uint32_t i = 0; i < N_POINTLIGHTS; ++i) {
//					if (points[i] == nullptr) {
//						points[i] = l;
//						pDist[i] = distance;
//						break;
//					} else {
//						if (distance < pDist[i]) {
//							PointLight* pTemp = points[i];
//							float dTemp = pDist[i];
//							points[i] = l;
//							pDist[i] = distance;
//						}
//					}
//				}
//			} else if (light->lightType == LightType::Spot) {
//				SpotLight* l = reinterpret_cast<SpotLight*>(light);
//				float distance = glm::length(l->position - objectPos);
//				for (uint32_t i = 0; i < N_SPOTLIGHTS; ++i) {
//					if (spots[i] == nullptr) {
//						spots[i] = l;
//						sDist[i] = distance;
//						break;
//					} else {
//						if (distance < sDist[i]) {
//							SpotLight* lTemp = spots[i];
//							float dTemp = sDist[i];
//							spots[i] = l;
//							sDist[i] = distance;
//						}
//					}
//				}
//			}
//		}

//		// Pass light to shader
//		if (dir != nullptr) {
//			dir->bind(shader);
//			lightCount.x++;
//		}

//		for (uint32_t i = 0; i < N_POINTLIGHTS; ++i) {
//			if (points[i] != nullptr) {
//				points[i]->bind(shader, i);
//				lightCount.y++;
//			} else
//				break;
//		}
//		for (uint32_t i = 0; i < N_SPOTLIGHTS; ++i) {
//			if (spots[i] != nullptr) {
//				spots[i]->bind(shader, i);
//				lightCount.z++;
//			} else
//				break;
//		}
//		shader->setIVec3("uLightCount", lightCount);
//	}
//}
//std::vector<Light*>& GetLights() {
//	return lights;
//}