
#include "Engone/Engone.h"

#include "Engone/EventModule.h"
#include "Engone/AssetModule.h"
const char* test2dGLSL = {
#include "Engone/Tests/test2d.glsl"
};
#include "Engone/Tests/BasicRendering.h"

// used to get the sizeof context because it affects getMemory
//#include "asio.hpp"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; // use the graphics card
}
namespace engone {
	TrackerId Engone::trackerId = "Engone";

	static const char* guiShaderSource = {
#include "Engone/Shaders/gui.glsl"
	};
	static const char* objectSource = {
#include "Engone/Shaders/object.glsl"
	};
	static const char* collisionSource = {
#include "Engone/Shaders/collision.glsl"
	};

#define INSTANCE_LIMIT 1000u

	Engone::Engone() {}
	Engone::~Engone() {
		for (int i = 0; i < m_applications.size();i++) {
			delete m_applications[i];
		}
		m_applications.clear();
		for (int i = 0; i < m_lights.size(); i++) {
			delete m_lights[i];
		}
		m_lights.clear();
		DestroyNetworking();
	}
	void Engone::start() {
		
		// Filters don't work with multithreading
		//log::out.setConsoleFilter(log::out.consoleFilter() | log::Disable);
		//GetTracker().printMemory();
		//GetTracker().printInfo();
		//log::out.setConsoleFilter(log::out.consoleFilter() & (~log::Disable));

		float printTime = 0;
		double lastTime = GetSystemTime();
		while (true) {
			double now = GetSystemTime();
			double delta = now - lastTime;
			lastTime = now;
			//const double fixed = 1 / 144.f;
			//m_runtimeStats.updateTime = fixed;
			if (m_flags & EngoneFixedLoop) {
				double sleeping = m_runtimeStats.frameTime - delta;
				//log::out << delta <<" "<< sleeping <<"\n";
				delta = m_runtimeStats.frameTime;
				if (sleeping > 0) {
					m_runtimeStats.sleepTime += sleeping;
					std::this_thread::sleep_for(std::chrono::microseconds((int)(sleeping * 1000000)));
					lastTime = GetSystemTime(); // this doesn't time accurate.
				}
			}

			// high delta will cause a series of consecuative updates which will freeze the apps. This is unwanted.
			// Limit delta to fix it.
			double limit = 8 * m_runtimeStats.updateTime;
			if (delta > limit) delta = limit;

			m_runtimeStats.runTime += delta;
			m_runtimeStats.update_accumulator += delta;
			m_runtimeStats.frame_accumulator += delta;
			printTime += delta;

			m_runtimeStats.nextSample(delta);
			
			for (uint32_t appIndex = 0; appIndex < m_applications.size(); ++appIndex) {
				Application* app = m_applications[appIndex];
				// Close window and applications if needed.
				for (uint32_t winIndex = 0; winIndex < app->getAttachedWindows().size(); ++winIndex) {
					Window* win = app->getWindow(winIndex);
					// Potentially close window
					if (!win->isOpen()) {
						GetTracker().untrack(win);
						delete win;
						app->getAttachedWindows().erase(app->getAttachedWindows().begin() + winIndex);
						--winIndex;
						continue;
					}
				}
				if (app->isStopped()) { // app needs to be stopped manually (Application::stop)

					// if you store any vertex buffers in the application they should have been destroyed by the window. (since the window destroys the context they were on)
					// app should be safe to delete
					GetTracker().untrack({ m_appIds[appIndex],m_appSizes[appIndex],app });
					delete app;
					 //FEATURE: instead of 3 vectors, use one vector<Tracker::TrackClass>
					m_applications.erase(m_applications.begin() + appIndex);
					m_appSizes.erase(m_appSizes.begin() + appIndex);
					m_appIds.erase(m_appIds.begin() + appIndex);
					--appIndex;
					continue;
				}
			}
			// NOTE: if you close one application and lets say a player object uses a camera inside it's update function. Since the camera exists in the renderer of a window which was deleted
			//   because one application closed, THEN the update function would crash because the camera is invalid. This doesn't happen if you use GetActiveWindow and check if it's nullptr
			//   but if you have a specific reference to a window that was closed wierd stuff could happen. And this only happens if you have two applications running at some point.
			//   Because if you have one then the code below will just break and the update function won't be called.
			// 
			// NOTE: Hello again, a clarification: Window* m_window is variable i usually create inside applications. When the window closes, this pointer points to invalid
			//	memory. The point here is... don't forget to set m_window to nullptr in onClose.

			if (m_applications.size() == 0) // might as well quit
				break;
			//Timer tim("update");
			while (m_runtimeStats.update_accumulator >= m_runtimeStats.updateTime|| m_flags & EngoneFixedLoop)
			{
				m_runtimeStats.update_accumulator -= m_runtimeStats.updateTime;

				m_runtimeStats.incrUpdates();
				LoopInfo info = { m_runtimeStats.updateTime };
				if (m_flags & EngoneFixedLoop)
					info.timeStep = m_runtimeStats.frameTime;
				//LoopInfo info = { delta, nullptr };
				// 
				//update(info);
				for (uint32_t appIndex = 0; appIndex < m_applications.size(); ++appIndex) {
					Application* app = m_applications[appIndex];
					info.app = app;

					// IsKeyPressed should work on current app and the first window.
					if (app->getWindow(0))
						app->getWindow(0)->setActiveContext();

					info.window = app->getWindow(0);

					app->update(info);
					//Timer tim("update");
					update(info); // should maybe be moved outside loop?
					//tim.stop();
					// update reset for all windows.
					// The global variable IsKeyPressed only works for the active window
					// which is mainWindow which is the first window attached to application.
					// If you want isKeyPressed for a specific window you do
					// window->isKeyPressed instead where window is you window of choice.
					for (uint32_t winIndex = 0; winIndex < app->getAttachedWindows().size(); ++winIndex) {
						app->getWindow(winIndex)->resetEvents();
					}
				}
				if (m_flags & EngoneFixedLoop)
					break;
			}
			//tim.stop();
			//Timer timer("frame");
			if (m_runtimeStats.frame_accumulator >= m_runtimeStats.frameTime||m_flags&EngoneFixedLoop) {
			//if(true){
				m_runtimeStats.frame_accumulator -= m_runtimeStats.frameTime;
				m_runtimeStats.incrFrames();
				for (uint32_t appIndex = 0; appIndex < m_applications.size(); ++appIndex) {
					Application* app = m_applications[appIndex];
					app->m_renderingWindows = true;
					//if (appIndex == 1) continue;
					for (uint32_t winIndex = 0; winIndex < app->getAttachedWindows().size(); ++winIndex) {
						Window* win = app->getWindow(winIndex);
						if (!win->isOpen()) continue;
						float interpolation = m_runtimeStats.update_accumulator / m_runtimeStats.updateTime; // Note: update accumulator should be there.
						LoopInfo info = { m_runtimeStats.updateTime,app,win,interpolation };
						if (m_flags & EngoneFixedLoop) {
							info.timeStep = m_runtimeStats.frameTime;
							info.interpolation = 0;
						}
						win->setActiveContext();

						if (!win->getStorage()->getGraphicProcessors().empty()) {
							win->getStorage()->getGraphicProcessors()[0]->process();
						}

						// Important setup
						// DONE IN Window::setActiveContext
						glViewport(0, 0, (uint32_t)GetWidth(), (uint32_t)GetHeight());
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glDepthFunc(GL_LESS);
						glEnable(GL_CULL_FACE);
						glClearColor(0.15f, 0.18f, 0.18f, 1.f);
						//glClearColor(1.15f, 0.18f, 0.18f, 1.f);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

						// 3d stuf
						float ratio = GetWidth() / GetHeight();
						if(isfinite(ratio))
							win->getRenderer()->setProjection(ratio);

						app->render(info);

						render(info);
						GetActiveRenderer()->render(info);

						glfwSwapInterval(0); // turning off vsync?
						//Timer swapT("glSwapBuffers");
						glfwSwapBuffers(win->glfw());
						//swapT.stop();
						// frame reset
						win->resetEvents();
						char endChr;
						while (endChr = win->pollChar()) {
							//log::out << "endpoll " << endChr << "\n";
						}
					}
					app->m_renderingWindows = false;
				}
			} else {
				double sleepTime = m_runtimeStats.frameTime - m_runtimeStats.frame_accumulator;
				m_runtimeStats.sleepTime += sleepTime;
				std::this_thread::sleep_for(std::chrono::microseconds((int)(sleepTime* 1000000.0)));
			}
			//timer.stop();

			if (printTime > 0.7f) {
				printTime -=0.7f;
				//m_runtimeStats.print(RuntimeStats::PrintSamples);
			}

			m_runtimeStats.totalLoops++;
			//log::out << "loop " << m_runtimeStats.totalLoops << "\n";
			//Timer poll("glPollEvents");
			glfwPollEvents();
			//poll.stop();
		}
		glfwTerminate();
		 
		// doesn't have to be a memory leak, it depends on the users code.
		// they may have allocated memory in global scope and never deleted it. But only if they used the tracker functions.
		//GetTracker().clear();
		
		GetTracker().printInfo();
		// The tracker may a little tricky to analyize correctly sometimes. Just don't get too fixated on it.

		// this means that the Memory usage is somewhat high because of io_context (and glfw, and reactphysics)
		
		DestroyNetworking();

		uint32_t expectedMem = 0;
		if (GetTracker().getMemory() > expectedMem) {
			log::out << "Memory leak? "<<GetTracker().getMemory()<<" expected "<<expectedMem << "\n";
			GetTracker().printMemory();
		} else {
			log::out << "No tracked memory leak\n";
		}
	}


	//FrameBuffer depthBuffer;
	//glm::mat4 lightProjection;
	//FrameBuffer& GetDepthBuffer() {
	//	return depthBuffer;
	//}

	EventType DrawOnResize(Event& e) {
		//glViewport(0, 0, Width(), Height());
		//glClearColor(1, 0, 0, 1);
		//glClear(GL_COLOR_BUFFER_BIT);
		//UpdateEngine(1 / 40.f);
		// 
		// maybe using refresh rate instead?
		//update(1 / 40.f);
		//RenderEngine(0);
		//render(0);



		//glfwSwapBuffers(GetActiveWindow()->glfw());

		return EventNone;
	}
	void Engone::update(LoopInfo& info) {
		if(info.app->getWorld())
			info.app->getWorld()->update(info);
	}
	float testTime = 0;
	void Engone::render(LoopInfo& info) {
		//test::TestParticles(info);
		EnableDepth();
		//if (IsKeyDown(GLFW_KEY_K)) {
		//Shader* blur = info.window->getAssets()->get<ShaderAsset>("blur");
		ShaderAsset* blur =  info.window->getStorage()->get<ShaderAsset>("blur");
		//Shader* blur = info.window->getAssets()->get<ShaderAsset>("blur");
		blur = nullptr;
		if (blur) {
			float detail = 1 / 3.f;
			//float detail = 1;
			glViewport(0, 0, GetWidth() * detail, GetHeight() * detail);
			if (!frameBuffer.initialized()) {
				frameBuffer.initBlur(GetWidth() * detail, GetHeight() * detail);
			}
			frameBuffer.bind();
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			////}
				//glEnable(GL_POINT_SMOOTH);
		}
		else {
		}
		//EnableBlend(); <- done in particle render
		if (info.window->getParent()->getWorld()) {
			auto& groups = info.window->getParent()->getWorld()->getParticleGroups();
			for (int i = 0; i < groups.size(); i++) {
				groups[i]->render(info);
			}
		}
		//EnableDepth();
		if (blur) {
			//glDisable(GL_POINT_SMOOTH);
			//if (IsKeyDown(GLFW_KEY_K)) {
			frameBuffer.unbind();
			glViewport(0, 0, GetWidth(), GetHeight());
			//
			//Shader* blur = info.window->getAssets()->get<Shader>("blur");
			ShaderAsset* blur = info.window->getStorage()->get<ShaderAsset>("blur");
			blur->bind();
			blur->setInt("uTexture", 0);
			blur->setVec2("uInvTextureSize", { 1.f / frameBuffer.getWidth(), 1.f / frameBuffer.getHeight() });
			frameBuffer.bindTexture();
			glDisable(GL_DEPTH_TEST); // these depend on what you want, don't disable if you use fragdepth
			info.window->getRenderer()->DrawQuad(info);

			//EnableDepth();
			frameBuffer.blitDepth(GetWidth(), GetHeight()); // not this if frag depth
		}
		EnableDepth();
		//}
		//glm::mat4 lightView = glm::lookAt({ -2,4,-1 }, glm::vec3(0), { 0,1,0 });
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
		//Shader* objectShader = GetAsset<Shader>("object");
		//if (objectShader != nullptr) {
		//	objectShader->bind();

		//	objectShader->setInt("shadow_map", 0);
		//	objectShader->setMat4("uLightSpaceMatrix", GetLightProj() * lightView);
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, GetDepthBuffer().texture);

		//	MeshAsset* asset = GetAsset<MeshAsset>("Axis/Cube.031");

		//	for (uint32_t j = 0; j < asset->materials.size(); ++j) {// Maximum of 4 materials
		//		asset->materials[j]->bind(objectShader, j);
		//	}
		//	objectShader->setMat4("uTransform", glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));
		//	asset->vertexArray.draw(&asset->indexBuffer);


		//	//objectShader->SetMatrix("uTransform", glm::mat4(1));
		//	//testBuffer.Draw();
		//}
#ifdef ENGONE_PHYSICS
		renderObjects(info);
#else
		// cannot render objects without physics
		if (info.window->getParent()->getGround()->getObjects().size() > 0)
			log::out << log::RED << "Cannot render objects without physics\n";
#endif
		/*
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

		//-- Debug
		EnableBlend();
		//Shader* gui = GetAsset<Shader>("gui");
		//Shader* gui = info.window->getAssets()->get<Shader>("gui");
		//gui->bind();
		//gui->setVec2("uPos", { 0,0 });
		//gui->setVec2("uWindow", { GetWidth(), GetHeight() });
		//gui->setVec2("uSize", { 1,1 });
		//gui->setVec4("uColor", 1, 1, 1, 1);
		//gui->setInt("uColorMode", 1);
		//Camera* cam = info.window->getRenderer()->getCamera();
		//// Note that this is the camera's location. Not the player's.
		//info.window->getRenderer()->DrawString(info.window->getAssets()->get<Font>("consolas"),
		//	std::to_string(cam->getPosition().x) + " " + std::to_string(cam->getPosition().y) + " " +
		//	std::to_string(cam->getPosition().z), false, 50, 300, 50, -1);

		// Physics debug
#ifdef ENGONE_PHYSICS
		rp3d::PhysicsWorld* world = info.window->getParent()->getWorld()->m_pWorld;
		if (world) {
			if (world->getIsDebugRenderingEnabled()) {
				rp3d::DebugRenderer& debugRenderer = world->getDebugRenderer();
				//log::out << debugRenderer.getNbLines() << " " << debugRenderer.getNbTriangles() << "\n";
				for (int i = 0; i < debugRenderer.getNbLines(); i++) {
					auto& line = debugRenderer.getLinesArray()[i];
					info.window->getRenderer()->DrawLine(*(glm::vec3*)&line.point1, *(glm::vec3*)&line.point2);
				}
				//Shader* shad = info.window->getAssets()->get<Shader>("lines3d");
				//shad->bind();
				//shad->setVec3("uColor", { 0.9,0.2,0.2 });
				for (int i = 0; i < debugRenderer.getNbTriangles(); i++) {
					auto& tri = debugRenderer.getTrianglesArray()[i];
					info.window->getRenderer()->DrawTriangle(*(glm::vec3*)&tri.point1, *(glm::vec3*)&tri.point2, *(glm::vec3*)&tri.point3);
				}
			}
		}
#endif
	}
	bool warnNoLights = false;
	void Engone::renderObjects(LoopInfo& info) {
		AssetStorage* assets = info.window->getStorage();
		//Assets* assets = info.window->getAssets();
		Renderer* renderer = info.window->getRenderer();
		Camera* camera = renderer->getCamera();
		EnableDepth();

		if (m_lights.size() == 0 && !warnNoLights) {
			warnNoLights = true;
			// app without 3d rendering doesn't care about this
			//log::out << log::YELLOW<<"Warning: rendering with no lights...\n";
		}
		std::unordered_map<MeshAsset*, std::vector<glm::mat4>> normalObjects;
		EngineWorld* world = info.window->getParent()->getWorld();
		//std::vector<EngineObject*>& objects = ground->getObjects();
		world->lock();

		EngineObjectIterator iterator = world->getIterator();
		EngineObject* obj = 0;
		while (obj = iterator.next()) {
			glm::mat4 modelMatrix = ToMatrix(obj->rigidBody->getTransform());

			if (!obj->modelAsset) continue;
			if (!obj->modelAsset->valid()) continue;

			Animator* animator = &obj->animator;

			// Get individual transforms
			std::vector<glm::mat4> transforms = obj->modelAsset->getParentTransforms(nullptr);

			/*
			if ((transform->position.x-camPos.x)* (transform->position.x - camPos.x)
				+(transform->position.y - camPos.y)*(transform->position.y - camPos.y)+
				(transform->position.z - camPos.z)*(transform->position.z - camPos.z)>200*200) {
				continue;
			}
			*/
				// Draw instances
			for (uint32_t i = 0; i < obj->modelAsset->instances.size(); ++i) {
				AssetInstance& instance = obj->modelAsset->instances[i];
				//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
				if (instance.asset->type == MeshAsset::TYPE) {
					MeshAsset* asset = (MeshAsset*)instance.asset;

					if (asset->meshType == MeshAsset::MeshType::Normal) {
						glm::mat4 out;

						//if (animator->asset)
						out = modelMatrix * transforms[i] * instance.localMat;
						//else
							//out = modelMatrix * instance.localMat;


						//out = glm::rotate(-20 * glm::pi<float>() / 180.f, glm::vec3(1, 0, 0));

						if (normalObjects.count(asset) > 0) {
							normalObjects[asset].push_back(out);
						} else {
							normalObjects[asset] = std::vector<glm::mat4>();
							normalObjects[asset].push_back(out);
						}
					}
				}
			}
		}
		world->unlock();
		//world->m_mutex.unlock();
		ShaderAsset* shader = assets->get<ShaderAsset>("object");
		if (!shader->getError()) {
			shader->bind();
			renderer->updateProjection(shader);
			shader->setVec3("uCamera", camera->getPosition());

			bindLights(shader, { 0,0,0 });
			shader->setMat4("uTransform", glm::mat4(0)); // zero in mat4 means we do instanced rendering. uTransform should be ignored
			for (auto& [asset, vector] : normalObjects) {
				//if (asset->meshType == MeshAsset::MeshType::Normal) {
				for (uint32_t j = 0; j < asset->materials.size(); ++j) {// Maximum of 4 materials
					asset->materials[j]->bind(shader, j);
				}
				//}
				uint32_t remaining = vector.size();
				while (remaining > 0) {
					uint32_t batchAmount = std::min(remaining,Renderer::INSTANCE_BATCH);
					
					renderer->instanceBuffer.setData(batchAmount * sizeof(glm::mat4), (vector.data() + vector.size()-remaining));

					asset->vertexArray.selectBuffer(3, &renderer->instanceBuffer);

					asset->vertexArray.draw(&asset->indexBuffer, batchAmount);
					remaining -= batchAmount;
				}
			}
		}
		shader = assets->get<ShaderAsset>("armature");
		world->lock();
		if (!shader->getError()) {
			shader->bind();
			renderer->updateProjection(shader);
			shader->setVec3("uCamera", camera->getPosition());

			EngineObjectIterator iterator = world->getIterator();
			EngineObject* obj;
			while(obj = iterator.next()){
			//for (int oi = 0; oi < objects.size(); ++oi) {
				//EngineObject* obj = objects[oi];

				if (!obj->modelAsset) continue;
				if (!obj->modelAsset->valid()) continue;


				glm::mat4 modelMatrix = ToMatrix(obj->rigidBody->getTransform());

				Animator* animator = &obj->animator;

				bindLights(shader, modelMatrix[3]);

				// Get individual transforms
				std::vector<glm::mat4> transforms;
				transforms = obj->modelAsset->getParentTransforms(nullptr);

				//-- Draw instances
				AssetInstance* armatureInstance = nullptr;
				ArmatureAsset* armatureAsset = nullptr;
				if (transforms.size() == obj->modelAsset->instances.size()) {
					for (uint32_t i = 0; i < obj->modelAsset->instances.size(); ++i) {
						AssetInstance& instance = obj->modelAsset->instances[i];

						if (instance.asset->type == AssetArmature) {
							armatureInstance = &instance;
							armatureAsset = (ArmatureAsset*)instance.asset;
						} else if (instance.asset->type == AssetMesh) {
							if (!armatureInstance)
								continue;

							MeshAsset* meshAsset = (MeshAsset*)instance.asset;
							if (meshAsset->meshType == MeshAsset::MeshType::Boned) {
								if (instance.parent == -1)
									continue;

								std::vector<glm::mat4> boneMats;
								std::vector<glm::mat4> boneTransforms = obj->modelAsset->getArmatureTransforms(animator, transforms[i], armatureInstance, armatureAsset, &boneMats);

								//-- For rendering bones
								//glm::mat4 basePos = modelMatrix* transforms[i];
								//glm::mat4 boneLast;
								//// one bone is missing. the first bone goes from origin to origin which makes it seem like a bone is missing
								//for (int j = 0; j < boneMats.size(); j++) {
								//	glm::mat4 boneMat = basePos*boneMats[j];
								//	if(j!=0) // skip first boneLast 
								//		info.window->getRenderer()->DrawLine(boneLast[3], boneMat[3]);
								//	boneLast = boneMat;
								//}

								for (uint32_t j = 0; j < boneTransforms.size(); ++j) {
									shader->setMat4("uBoneTransforms[" + std::to_string(j) + "]", boneTransforms[j] * instance.localMat);
								}

								for (uint32_t j = 0; j < meshAsset->materials.size(); ++j) {// Maximum of 4 materials
									meshAsset->materials[j]->bind(shader, j);
								}
								shader->setMat4("uTransform", modelMatrix * transforms[i]);
								meshAsset->vertexArray.draw(&meshAsset->indexBuffer);
							}
						}
					}
				}
			}
		}
		world->unlock();

		//}
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

		//for (auto [a, b] : normalObjects) {
		//	b.clear();
		//}
		//normalObjects.clear();
	}
	void Engone::addLight(Light* l) {
		m_lights.push_back(l);
	}
	void Engone::removeLight(Light* l) {
		for (uint32_t i = 0; i < m_lights.size(); ++i) {
			if (m_lights[i] == l) {
				m_lights.erase(m_lights.begin() + i);
				break;
			}
		}
	}
	void Engone::bindLights(Shader* shader, glm::vec3 objectPos) {
		if (shader != nullptr) {
			// List setup
			const int N_POINTLIGHTS = 4;
			const int N_SPOTLIGHTS = 2;

			DirLight* dir = nullptr;
			PointLight* points[N_POINTLIGHTS];
			float pDist[N_POINTLIGHTS];
			for (uint32_t i = 0; i < N_POINTLIGHTS; ++i) {
				points[i] = nullptr;
				pDist[i] = 9999;
			}

			SpotLight* spots[N_SPOTLIGHTS];
			float sDist[N_SPOTLIGHTS];
			for (uint32_t i = 0; i < N_SPOTLIGHTS; ++i) {
				spots[i] = nullptr;
				sDist[i] = 9999;
			}
			glm::ivec3 lightCount(0);

			// Grab the closest lights to the object
			for (uint32_t i = 0; i < m_lights.size(); ++i) {
				Light* light = m_lights[i];
				if (light->lightType == LightType::Direction) {
					dir = reinterpret_cast<DirLight*>(light);
				} else if (light->lightType == LightType::Point) {
					PointLight* l = reinterpret_cast<PointLight*>(light);
					float distance = glm::length(l->position - objectPos);
					for (uint32_t i = 0; i < N_POINTLIGHTS; ++i) {
						if (points[i] == nullptr) {
							points[i] = l;
							pDist[i] = distance;
							break;
						} else {
							if (distance < pDist[i]) {
								PointLight* pTemp = points[i];
								float dTemp = pDist[i];
								points[i] = l;
								pDist[i] = distance;
							}
						}
					}
				} else if (light->lightType == LightType::Spot) {
					SpotLight* l = reinterpret_cast<SpotLight*>(light);
					float distance = glm::length(l->position - objectPos);
					for (uint32_t i = 0; i < N_SPOTLIGHTS; ++i) {
						if (spots[i] == nullptr) {
							spots[i] = l;
							sDist[i] = distance;
							break;
						} else {
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
				dir->bind(shader);
				lightCount.x++;
			}

			for (uint32_t i = 0; i < N_POINTLIGHTS; ++i) {
				if (points[i] != nullptr) {
					points[i]->bind(shader, i);
					lightCount.y++;
				} else
					break;
			}
			for (uint32_t i = 0; i < N_SPOTLIGHTS; ++i) {
				if (spots[i] != nullptr) {
					spots[i]->bind(shader, i);
					lightCount.z++;
				} else
					break;
			}
			shader->setIVec3("uLightCount", lightCount);
		}
	}
}