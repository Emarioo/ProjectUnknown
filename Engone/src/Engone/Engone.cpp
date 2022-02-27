#include "gonpch.h"

#include "Engone.h"

#include "glm/gtx/matrix_decompose.hpp"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

namespace engone {

	static const std::string guiShaderSource = {
#include "Shaders/gui.glsl"
	};
	static const std::string objectSource = {
#include "Shaders/object.glsl"
	};
	static const std::string armatureSource = {
#include "Shaders/armature.glsl"
	};
	static const std::string collisionSource = {
#include "Shaders/collision.glsl"
	};

	static std::function<void(double)> update = nullptr;
	static std::function<void(double)> render = nullptr;

	FrameBuffer depthBuffer;
	glm::mat4 lightProjection;
	FrameBuffer& GetDepthBuffer() {
		return depthBuffer;
	}
	glm::mat4& GetLightProj() {
		return lightProjection;
	}

	static glm::mat4 projMatrix;
	static glm::mat4 viewMatrix;
	static float fov, zNear, zFar;

	static int lastMouseX = -1, lastMouseY = -1;
	static double cameraSensitivity = 0.1;

	EventType FirstPerson(Event& e) {
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

		UpdateEngine(1 / 40.f);
		update(1 / 40.f);
		RenderEngine(0);
		render(0);

		glfwSwapBuffers(GetWindow());

		return EventType::None;
	}
	bool glfwIsActive = false;
	void InitEngone() {
		// There is this bug with glfw where it freezes. This will tell you if it has.
		std::thread tempThread = std::thread([] {
			for (int i = 0; i < 20; i++) {
				std::this_thread::sleep_for((std::chrono::milliseconds)100);
				if (glfwIsActive) {
					return;
				}
			}
			std::cout << "GLFW has frozen. :( \n";
			});
		if (!glfwInit()) {
			std::cout << "Glfw Init error!" << std::endl;
			return;
		}
		glfwIsActive = true;
		tempThread.join();

		InitWindow();

		if (glewInit() != GLEW_OK) {
			std::cout << "Glew Init Error!" << std::endl;
			return;
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		//glEnable(GL_FRAMEBUFFER_SRGB);

		// Gui stuff
		AddAsset<Shader>("gui", new Shader(guiShaderSource, true));

		engone::SetState(GameState::RenderGame, true);
		engone::SetState(GameState::RenderGui, true);

		AddAsset<Shader>("object", new Shader(objectSource, true));
		AddAsset<Shader>("armature", new Shader(armatureSource, true));
		AddAsset<Shader>("collision", new Shader(collisionSource, true));
		//AddAsset<MaterialAsset>("defaultMaterial", new MaterialAsset());

		InitEvents(GetWindow());
		InitGui();
		InitRenderer();
		//ReadOptions();

		AddListener(new Listener(EventType::Move, 9998, FirstPerson));
		AddListener(new Listener(EventType::Resize, 9999, DrawOnResize));

		//UpdateKeyboard(false);

		depthBuffer.Init();
		float near_plane = 1.f, far_plane = 20.5f;
		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);

		fov = 90.f;
		zNear = 0.1f;
		zFar = 400.f;
		SetProjection(GetWidth() / GetHeight());
	}
	void UninitEngone() {

	}

	void SetProjection(float ratio) {
		projMatrix = glm::perspective(fov, ratio, zNear, zFar);
	}
	void UpdateViewMatrix(double lag) {
		viewMatrix = glm::inverse(
			glm::translate(glm::mat4(1.0f), GetCamera()->position) *
			glm::rotate(GetCamera()->rotation.y, glm::vec3(0, 1, 0)) *
			glm::rotate(GetCamera()->rotation.x, glm::vec3(1, 0, 0))
		);
	}
	void UpdateProjection(Shader* shader) {
		if (shader != nullptr)
			shader->SetMatrix("uProj", projMatrix * viewMatrix);
	}

	bool TestCollision(Collider* c1, Collider* c2) {
		bool colliding = false;
		float time = 0;

		Transform* t1 = c1->entity.getComponent<Transform>();
		Transform* t2 = c2->entity.getComponent<Transform>();
		Physics* p1 = c1->entity.getComponent<Physics>();
		Physics* p2 = c2->entity.getComponent<Physics>();

		if (c1->asset->colliderType == ColliderAsset::Type::Sphere &&
			c2->asset->colliderType == ColliderAsset::Type::Sphere) {

			glm::vec3& rot1 = c1->asset->sphere.position;
			float dist = glm::length(c1->asset->sphere.position - c2->asset->sphere.position);
			if (dist < c1->asset->sphere.radius + c2->asset->sphere.radius) {
				colliding = true;
				time = 1;
			}

		} else if (c1->asset->colliderType == ColliderAsset::Type::Cube &&
			c2->asset->colliderType == ColliderAsset::Type::Cube) {

			// get collision points
			glm::vec3 move = t1->position - p1->lastPosition;
			if (move.x == 0 && move.y == 0 && move.z == 0) {
				move = t2->position - p2->lastPosition;
				if (move.x == 0 && move.y == 0 && move.z == 0)
					return false;
				Collider* temp = c1;
				c1 = c2;
				c2 = temp;
			}

			glm::vec3 diff = p1->lastPosition - p2->lastPosition;
			glm::vec3 point1 = c1->position - c1->scale / 2.f;
			glm::vec3 point2 = c2->position - c2->scale / 2.f;

			point1.x += (diff.x < 0) ? c1->scale.x : 0;
			point1.y += (diff.y < 0) ? c1->scale.y : 0;
			point1.z += (diff.z < 0) ? c1->scale.z : 0;
			point2.x += (diff.x < 0) ? 0 : c2->scale.x;
			point2.y += (diff.y < 0) ? 0 : c2->scale.y;
			point2.z += (diff.z < 0) ? 0 : c2->scale.z;

			float tx = (point2.x - point1.x) / move.x;
			float ty = (point2.y - point1.y) / move.y;
			float tz = (point2.z - point1.z) / move.z;

			glm::vec3 s1 = c1->scale;
			glm::vec3 v1 = c1->position - s1 / 2.f;
			glm::vec3 s2 = c2->scale;
			glm::vec3 v2 = c2->position - s2 / 2.f;

			if (v1.x + s1.x > v2.x && v1.x < v2.x + s2.x &&
				v1.y + s1.y > v2.y && v1.y < v2.y + s2.y &&
				v1.z + s1.z > v2.z && v1.z < v2.z + s2.z) {

				if (abs(tx) < abs(ty) && abs(tx) < abs(tz)) {
					t1->position.x += tx * move.x;
					p1->velocity.x = 0;
					return true;
				} else if (abs(ty) < abs(tz)) {
					t1->position.y += ty * move.y;
					p1->velocity.y = 0;
					return true;
				} else {
					t1->position.z += tz * move.z;
					p1->velocity.z = 0;
					return true;
				}
			}
		}
		return false;
	}

	void UpdateObjects(float delta) 
	{
		std::vector<Collider> colliders;
	
		// Space partitioning?

		//-- Update scriptables
		/*EntityIterator scriptables = GetEntityIterator(ComponentEnum::Scriptable);
		while(scriptables) {
			scriptables.get<Scriptable>()->entity->Update(delta);
		}*/

		//-- Movement and physics
		EntityIterator iterator = GetEntityIterator(ComponentEnum::Transform|ComponentEnum::Physics|ComponentEnum::Model);
		while (iterator) {
			Transform* transform = iterator.get<Transform>();
			Physics* physics = iterator.get<Physics>();
			Model* modelC = iterator.get<Model>();
			Scriptable* scriptable = iterator.get<Scriptable>();

			physics->lastPosition = transform->position;

			if (scriptable)
				scriptable->entity->Update(delta);

			if (physics->movable) {
				physics->velocity.y += physics->gravity * delta;
				transform->position += physics->velocity * delta;
			}

			if (modelC->modelAsset != nullptr) {
				ModelAsset* model = modelC->modelAsset;

				// Get individual transforms
				std::vector<glm::mat4> transforms;
				Animator* animator = iterator.get<Animator>();
				if(animator!=nullptr)
					model->GetParentTransforms(animator, transforms);

				for (int j = 0; j < model->instances.size(); j++) {
					AssetInstance& instance = model->instances[j];

					if (instance.asset->type == AssetType::Collider) {
						ColliderAsset* asset = instance.asset->cast<ColliderAsset>();

						glm::mat4 matrix;
						if(animator!=nullptr)
							matrix = glm::translate(transform->position) * transforms[j] * instance.localMat;
						else
							matrix = glm::translate(transform->position) * instance.localMat;


						matrix = glm::scale(matrix, asset->cube.scale);

						colliders.push_back({ asset, iterator.getEntity() });
						glm::vec3 skew;
						glm::vec4 persp;
						glm::decompose(matrix, colliders.back().scale, colliders.back().rotation, colliders.back().position, skew, persp);

						if (asset->colliderType == ColliderAsset::Type::Sphere) {
							colliders.back().position += asset->sphere.position;
						} else if (asset->colliderType == ColliderAsset::Type::Cube) {
							colliders.back().position += asset->cube.position;
						}
					}
				}
			}
		}

		//-- Collision


		// Pre physics
		//for (int i = 0; i < GetObjects().size(); i++) {
		//	GameObject* object = GetObjects()[i];

		//	if (object->physics.m_isActive) {
		//		// reset triggering
		//		object->physics.m_isTriggered = false;

		//		object->physics.lastPosition = object->physics.position;
		//		
		//		object->Update(delta);
		//		if (object->physics.m_isMovable) {
		//			object->physics.velocity.y += object->physics.gravity * delta;
		//			object->physics.position += object->physics.velocity * delta;
		//			//log::out << object->physics.position << object->physics.velocity<< "\n";
		//		}

		//		if (object->GetModel() != nullptr) {
		//			ModelAsset* model = object->GetModel();

		//			// Get individual transforms
		//			std::vector<glm::mat4> transforms;
		//			model->GetParentTransforms(object->animator, transforms);

		//			for (int j = 0; j < model->instances.size(); j++) {
		//				AssetInstance& instance = model->instances[j];

		//				if (instance.asset->type == AssetType::Collider) {
		//					ColliderAsset* asset = instance.asset->cast<ColliderAsset>();

		//					glm::mat4 matrix = glm::translate(object->physics.position) * transforms[j] * instance.localMat;
		//					
		//					matrix = glm::scale(matrix, asset->cube.scale);
		//					
		//					colliders.push_back({ asset, &object->physics, object });
		//					glm::vec3 skew;
		//					glm::vec4 persp;
		//					glm::decompose(matrix,colliders.back().scale,colliders.back().rotation,colliders.back().position,skew,persp);
		//					
		//					if (asset->colliderType == ColliderAsset::Type::Sphere) {
		//						colliders.back().position += asset->sphere.position;
		//					} else if (asset->colliderType == ColliderAsset::Type::Cube) {
		//						colliders.back().position += asset->cube.position;
		//					}
		//				}
		//			}
		//		}
		//	}
		//}

		//log::out << "col\n";
		while (colliders.size()>1) {
			Collider& c1 = colliders[0];
			for (int i = 1; i < colliders.size();i++) {
				Collider& c2 = colliders[i];
				//log::out << c1.asset->baseName << " " << c2.asset->baseName << "\n";
				if ((c1.position - c2.position).length() < 50) {
					
					bool yes = TestCollision(&c1,&c2);
					if (yes) {
						Scriptable* s1 = c1.entity.getComponent<Scriptable>();
						Scriptable* s2 = c2.entity.getComponent<Scriptable>();
						if(s1)
							s1->entity->OnCollision(c1, c2);
						if(s2)
							s2->entity->OnCollision(c2, c1);
					}
				}
			}
			// remove collider when done with calculations
			colliders.erase(colliders.begin());
		}
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

	void RenderObjects(double lag) {
		// Apply lag transformation on objects?

		EnableDepth();
		UpdateViewMatrix(lag);

		Shader* shader = GetAsset<Shader>("object");

		if (!shader->error) {
			shader->Bind();
			UpdateProjection(shader);
			shader->SetVec3("uCamera", GetCamera()->position + GetCamera()->velocity * (float)lag);

			EntityIterator iterator = GetEntityIterator(ComponentEnum::Transform|ComponentEnum::Model);
			while (iterator) {
				Transform* transform = iterator.get<Transform>();
				Model* modelC = iterator.get<Model>();
				Animator* animator = iterator.get<Animator>();

				if (modelC->modelAsset) {
					ModelAsset* model = modelC->modelAsset;
					log::out << model->baseName << "\n";
					log::out << animator << "\n";
					//BindLights(shader, transform->position);

					// Get individual transforms
					std::vector<glm::mat4> transforms;
					if (animator)
						model->GetParentTransforms(animator, transforms);
					
					// Draw instances
					for (int i = 0; i < model->instances.size(); i++) {
						AssetInstance& instance = model->instances[i];
						//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
						if (instance.asset->type == AssetType::Mesh) {
							MeshAsset* asset = instance.asset->cast<MeshAsset>();
							if (asset->meshType == MeshAsset::MeshType::Normal) {
								for (int j = 0; j < asset->materials.size() && j < 4; j++)// Maximum of 4 materials
								{
									asset->materials[j]->Bind(shader, j);
								}
								if(animator)
									shader->SetMatrix("uTransform", glm::translate(transform->position) * transforms[i] * instance.localMat);
								else
									shader->SetMatrix("uTransform", glm::translate(transform->position) * instance.localMat);
								asset->buffer.Draw();
							}
						}
					}
				}
			}
		}

		shader = GetAsset<Shader>("armature");
		if (!shader->error) {
			shader->Bind();
			UpdateProjection(shader);
			shader->SetVec3("uCamera", GetCamera()->position + GetCamera()->velocity * (float)lag);

			EntityIterator iterator = GetEntityIterator(ComponentEnum::Transform | ComponentEnum::Model);
			while (iterator.next()) {
				Transform* transform = iterator.get<Transform>();
				Model* modelC = iterator.get<Model>();
				Animator* animator = iterator.get<Animator>();
				if (modelC->modelAsset) {
					ModelAsset* model = modelC->modelAsset;

					//BindLights(shader, transform->position);

					// Get individual transforms
					std::vector<glm::mat4> transforms;
					if (animator)
						model->GetParentTransforms(animator, transforms);

					//-- Draw instances
					for (int i = 0; i < model->instances.size(); i++) {
						AssetInstance& meshInstance = model->instances[i];

						if (meshInstance.asset->type == AssetType::Mesh) {
							MeshAsset* meshAsset = meshInstance.asset->cast<MeshAsset>();
							if (meshAsset->meshType == MeshAsset::MeshType::Boned) {
								if (meshInstance.parent == -1)
									continue;

								AssetInstance& armatureInstance = model->instances[meshInstance.parent];
								if (armatureInstance.asset->type == AssetType::Armature) {
									ArmatureAsset* armatureAsset = armatureInstance.asset->cast<ArmatureAsset>();

									std::vector<glm::mat4> boneTransforms;
									if (animator)
										model->GetArmatureTransforms(animator, boneTransforms, transforms[i], armatureInstance, armatureAsset);

									for (int j = 0; j < boneTransforms.size(); j++) {
										shader->SetMatrix("uBoneTransforms[" + std::to_string(j) + "]", boneTransforms[j] * meshInstance.localMat);
									}

									for (int j = 0; j < meshAsset->materials.size() && j < 4; j++)// Maximum of 4 materials
									{
										meshAsset->materials[j]->Bind(shader, j);
									}
									if (animator) {
										shader->SetMatrix("uTransform", glm::translate(transform->position) * transforms[i]);
									} else {
										shader->SetMatrix("uTransform", glm::translate(transform->position));
									}
									meshAsset->buffer.Draw();
								}
							}
						}
					}
				}
			}
		}
		shader = GetAsset<Shader>("collision");
		if (shader != nullptr) {
			shader->Bind();
			UpdateProjection(shader);
			glLineWidth(2.f);
			shader->SetVec3("uColor", {0.05,0.9,0.1});

			EntityIterator iterator = GetEntityIterator(ComponentEnum::Transform | ComponentEnum::Model|ComponentEnum::Physics);
			while (iterator.next()) {
				Transform* transform = iterator.get<Transform>();
				Model* modelC = iterator.get<Model>();
				Animator* animator = iterator.get<Animator>();
				Physics* physics = iterator.get<Physics>();

				if (!physics->renderCollision)
					continue;

				if (modelC->modelAsset) {
					ModelAsset* model = modelC->modelAsset;

					// Get individual transforms
					std::vector<glm::mat4> transforms;
					if(animator)
						model->GetParentTransforms(animator, transforms);

					// Draw instances
					for (int i = 0; i < model->instances.size(); i++) {
						AssetInstance& instance = model->instances[i];
						//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
						if (instance.asset->type == AssetType::Collider) {
							ColliderAsset* asset = instance.asset->cast<ColliderAsset>();
							glm::vec3 offset=glm::vec3(0);
							if (asset->colliderType == ColliderAsset::Type::Sphere) {
								offset = asset->sphere.position;
							} else if (asset->colliderType == ColliderAsset::Type::Cube) {
								offset = asset->cube.position;
							}
							if(animator)
								shader->SetMatrix("uTransform", glm::translate(transform->position+offset) * transforms[i] * instance.localMat);
							else
								shader->SetMatrix("uTransform", glm::translate(transform->position+offset) * instance.localMat);

							if (asset->colliderType == ColliderAsset::Type::Sphere) {
								DrawSphere(asset->sphere.radius);
								
							}else if (asset->colliderType == ColliderAsset::Type::Cube) {
								DrawCube(asset->cube.scale.x, asset->cube.scale.y, asset->cube.scale.z);
							} else if (asset->colliderType == ColliderAsset::Type::Mesh) {

							}
						}
					}
				}
			}

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
			for (GameObject* o : GetObjects()) {// Draws a hitbox for every object by thowing all the lines from colliders into a vertex buffer
				if (o->renderHitbox && o->collisionComponent.isActive) {
					int atVec = 0;
					int atInd = 0;
					CollisionComponent& c = o->collisionComponent;
					// algorithm for removing line duplicates?
					// Adding hitboxes together?
					int startV = atVec;
					if (atVec + c.points.size() * 6 > 6 * vecLimit) {
						if (!reachedLimit)
							bug::out < bug::RED < "Line limit does not allow " < (atVec + c.points.size() * 6) < "/" < (6 * vecLimit) < " points" < bug::end;
						reachedLimit = true;
						continue;
					} else if (atInd + c.coll->quad.size() * 4 + c.coll->tri.size() * 3 > lineLimit) {
						if (!reachedLimit)
							bug::out < bug::RED < "Line limit does not allow " < (atInd + c.coll->quad.size() * 4 + c.coll->tri.size() * 3) < "/" < (lineLimit) < " lines:" < bug::end;
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
				if (false && o->renderComponent.model != nullptr) {
					if (o->renderComponent.model->armature != nullptr) {
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
							if (atVec + count * 12 > 6 * vecLimit) {
								if (!reachedLimit)
									bug::out < bug::RED < "Line limit does not allow " < (atVec + count * 12) < "/" < (6 * vecLimit) < " points" < bug::end;
								reachedLimit = true;
								continue;
							} else if (atInd + count > lineLimit) {
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
	void RenderEngine(double lag)
	{
		glViewport(0, 0, GetWidth(), GetHeight());

		EnableDepth();
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
						asset->materials[j]->Bind(objectShader, j);
					}
					objectShader->SetMatrix("uTransform", glm::translate(glm::mat4(1),glm::vec3(0,0,0)));
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
			RenderElements();
		}

		//-- Debug
		EnableBlend();
		Shader* gui = GetAsset<Shader>("gui");
		gui->SetVec2("uPos", { 0,0 });
		gui->SetVec2("uSize", { 1,1 });
		gui->SetVec4("uColor", 1, 1, 1, 1);
		gui->SetInt("uColorMode", 1);
		DrawString(GetAsset<Font>("consolas"), std::to_string(GetCamera()->position.x) + " " + std::to_string(GetCamera()->position.y) + " " + std::to_string(GetCamera()->position.z), false, 50, 300, 50, -1);
	}
	void UpdateEngine(double delta)
	{
		if (CheckState(GameState::RenderGame)) {
			//if (HasFocus() && !CheckState(GameState::Paused)) {
			UpdateObjects(delta);
			//}
		}
		UpdateTimers(delta);
		if (CheckState(GameState::RenderGui)) {
			UpdateElements(delta);
		}

		ResetEvents();
	}
	static std::vector<Timer> timers;
	void AddTimer(float time, std::function<void()> func) {
		timers.push_back({ time,func });
	}
	void UpdateTimers(float delta) {
		for (int i = 0; i < timers.size(); i++) { // Will this cause problems? Removing an element in a for loop
			timers[i].time -= delta;
			if (timers[i].time < 0) {
				timers[i].func();
				timers.erase(timers.begin() + i);
				i--;
			}
		}
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
				dir->Bind(shader);
				lightCount.x++;
			}

			for (int i = 0; i < N_POINTLIGHTS; i++) {
				if (points[i] != nullptr) {
					points[i]->Bind(shader, i);
					lightCount.y++;
				}
				else
					break;
			}
			for (int i = 0; i < N_SPOTLIGHTS; i++) {
				if (spots[i] != nullptr) {
					spots[i]->Bind(shader, i);
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
	void Start(std::function<void(double)> inUpdate, std::function<void(double)> inRender, double fps) {
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
		double lastSecond = previous;
		int FPS = 0;
		while (IsOpen()) {
			double current = GetAppTime();
			double elapsed = current - previous;
			//std::cout << elapsed << std::endl;
			previous = current;
			lag += elapsed;

			while (lag >= MS_PER_UPDATE) {
				//UpdateEngine(MS_PER_UPDATE);
				update(MS_PER_UPDATE);
				lag -= MS_PER_UPDATE;
				FPS++;
				if (current - lastSecond > 1) {
					//std::cout << (current - lastSecond) << std::endl;
					lastSecond = current;
					//SetWindowTitle(("Project Unknown  " + std::to_string(FPS) + " fps").c_str());
					FPS = 0;
				}
			}
			//RenderEngine(lag);
			render(lag);

			glfwSwapBuffers(GetWindow());
			glfwPollEvents();
		}
		// remove buffer and stuff or not?
		glfwTerminate();
	}
}