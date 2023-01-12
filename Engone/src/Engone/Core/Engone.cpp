
#include "Engone/Engone.h"

#include "Engone/EventModule.h"
#include "Engone/AssetModule.h"
const char* test2dGLSL = {
#include "Engone/Tests/test2d.glsl"
};
#include "Engone/Tests/BasicRendering.h"

#include "Engone/Core/ExecutionControl.h"
#include "Engone/Utilities/Thread.h"

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
		for (int i = 0; i < m_applications.size(); i++) {
			//delete m_applications[i];
			ALLOC_DELETE(Application, m_applications[i]);
		}
		m_applications.clear();
		for (int i = 0; i < m_lights.size(); i++) {
			//delete m_lights[i];
			ALLOC_DELETE(Light, m_lights[i]);
		}
		m_lights.clear();
		DestroyNetworking();
	}
	void Engone::addApplication(Application* app) {
		//T* app = new T(this);
		GetTracker().track(app);
		m_applications.push_back(app);
		m_appSizes.push_back(sizeof(Application));
		m_appIds.push_back(Application::trackerId);
	}
	std::vector<Application*>& Engone::getApplications() { return m_applications; }
	void Engone::start() {

		if (m_applications.size() == 0) // might as well quit
			return;

		while (true) {
			//manageThreading(); not finished

			manageNonThreading(); // deals with timers
		}

		// Filters don't work with multithreading
		//log::out.setConsoleFilter(log::out.consoleFilter() | log::Disable);
		//GetTracker().printMemory();
		//GetTracker().printInfo();
		//log::out.setConsoleFilter(log::out.consoleFilter() & (~log::Disable));
		
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
			log::out << "Memory leak? " << GetTracker().getMemory() << " expected " << expectedMem << "\n";
			GetTracker().printMemory();
		} else {
			log::out << "No tracked memory leak\n";
		}
	}
	void Engone::manageThreading() {
		for (Application* app : m_applications) {
			if (app->isMultiThreaded()) { // is supposed to be multithreaded
				if (!app->updateThread.isRunning()) {
					app->updateThread.init([&](void* arg) {
						while (true) {
							app->getExecTimer().step();
							LoopInfo info;
							info.app = app;
							info.window = nullptr;
							info.timeStep = app->getExecTimer().delta;
							app->getControl().execute(info, ExecutionControl::UPDATE);

							// In case execute takes 0 seconds the thread should sleep for a little bit
							// to ease the strain on the CPU. CPU going full throttle while just looping is unnecessary.
							engone::Sleep(0.1);
						}
						return 0;
					}, nullptr);
				}
				//glfwPollEvents in some thread

				// same for all windows
				//for () {

				//}
				// multithreading isn't running, do run it.
			}
		}
	}
	void Engone::manageNonThreading() {
		//-- Timers and stuff
		mainUpdateTimer.step(); // replace with one step for both? they can become unsynchrized otherwise
		mainRenderTimer.step();

		// keep expected FPS, UPS outside of timer and pass it in as argument?
		
		// function seems to to processing of all inputs and stuff
		// mutex lock for window input stuff
		// special thread for input or just after render thread
		glfwPollEvents(); // window refreh callback to redraw when resizing?

		//-- Update execution
		while (mainUpdateTimer.accumulate()) {
			for (Application* app : m_applications) {
				if (!app->isMultiThreaded()) {

					if (app->getWindow(0))
						app->getWindow(0)->setActiveContext();

					currentLoopInfo = { mainUpdateTimer.aimedDelta * m_runtimeStats.gameSpeed,app,app->getWindow(0),0 };

					app->update(currentLoopInfo);
					update(currentLoopInfo); // should maybe be moved outside loop?

					//app->getControl().execute(currentLoopInfo, ExecutionControl::UPDATE);

					for (uint32_t winIndex = 0; winIndex < app->getAttachedWindows().size(); ++winIndex) {
						app->getWindow(winIndex)->resetEvents(false);
					}
				}
			}
		}
		//-- Render execution
		if (mainRenderTimer.accumulate()) {
			for (Application* app : m_applications) {
				if (!app->isMultiThreaded()) {
					app->m_renderingWindows = true;
					for (Window* win : app->getAttachedWindows()) {
						if (!win->isOpen()) continue;

						double interpolation = m_runtimeStats.update_accumulator / m_runtimeStats.updateTime; // Note: update accumulator should be there.

						currentLoopInfo = { m_runtimeStats.frameTime * m_runtimeStats.gameSpeed,app,win,interpolation };

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

						// 3d stuff
						app->render(currentLoopInfo);

						render(currentLoopInfo);

						//app->getControl().execute(currentLoopInfo, ExecutionControl::RENDER);

						// orthogonal was set before hand
						win->getPipeline()->render(currentLoopInfo);

						glfwSwapInterval(0); // turning off vsync?
						//Timer swapT("glSwapBuffers");
						glfwSwapBuffers(win->glfw());
						//swapT.stop();
						// frame reset
						win->resetEvents(true);
						char endChr;
						while (endChr = win->pollChar()) {
							//log::out << "endpoll " << endChr << "\n";
						}
					}
					app->m_renderingWindows = false;
				}
			}
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
		for (int i = 0; i < info.app->m_worlds.size(); i++) {
			EngineWorld* world = info.app->m_worlds[i];
			world->update(info);
		}
	}
	float testTime = 0;
	void Engone::render(LoopInfo& info) {
		//test::TestParticles(info);
		EnableDepth();
		//if (IsKeyDown(GLFW_KEY_K)) {
		ShaderAsset* blur =  info.window->getStorage()->get<ShaderAsset>("blur");
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
		//GL_CHECK()
		//EnableBlend(); <- done in particle render
		for (int i = 0; i < info.app->m_worlds.size(); i++) {
			auto& groups = info.app->m_worlds[i]->getParticleGroups();
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
			CommonRenderer* renderer = GET_COMMON_RENDERER();
			renderer->drawQuad(info);

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
		//if(info.window->getParent()->getWorld())
		//	if(info.window->getParent()->getWorld()->getObjectCount())
		//		log::out << log::RED << "Cannot render objects without physics\n";
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
		CommonRenderer* renderer = GET_COMMON_RENDERER();
		if (renderer) {
			// Physics debug
#ifdef ENGONE_PHYSICS
			for (int i = 0; i < info.app->m_worlds.size(); i++) {
				rp3d::PhysicsWorld* world = info.app->m_worlds[i]->getPhysicsWorld();
				if (!world) continue;
				if (!world->getIsDebugRenderingEnabled()) continue;

				rp3d::DebugRenderer& debugRenderer = world->getDebugRenderer();
				//log::out << debugRenderer.getNbLines() << " " << debugRenderer.getNbTriangles() << "\n";
				for (int i = 0; i < debugRenderer.getNbLines(); i++) {
					auto& line = debugRenderer.getLinesArray()[i];
					renderer->drawLine(*(glm::vec3*)&line.point1, *(glm::vec3*)&line.point2);
				}
				//Shader* shad = info.window->getAssets()->get<Shader>("lines3d");
				//shad->bind();
				//shad->setVec3("uColor", { 0.9,0.2,0.2 });
				for (int i = 0; i < debugRenderer.getNbTriangles(); i++) {
					auto& tri = debugRenderer.getTrianglesArray()[i];
					renderer->drawTriangle(*(glm::vec3*)&tri.point1, *(glm::vec3*)&tri.point2, *(glm::vec3*)&tri.point3);
				}
			}
#endif
		}
		if (m_flags & EngoneEnableDebugInfo) {
			// Toggle Basic Info
			if (IsKeyDown(GLFW_KEY_F3) && IsKeyPressed(GLFW_KEY_B)) {
				if (m_flags & EngoneShowBasicDebugInfo)
					m_flags &= ~EngoneShowBasicDebugInfo;
				else
					m_flags |= EngoneShowBasicDebugInfo;
			}
			
			if (m_flags & EngoneShowBasicDebugInfo)
				renderBasicDebugInfo(info);
		}
	}
	bool warnNoLights = false;
	void Engone::renderObjects(LoopInfo& info) {
		AssetStorage* assets = info.window->getStorage();
		//Assets* assets = info.window->getAssets();
		//Renderer* renderer = info.window->getRenderer();
		CommonRenderer* renderer = GET_COMMON_RENDERER();
		if (!renderer) {
			log::out << log::RED << "Engone::RenderObjects : renderer is null\n";
			return;
		}
		Camera* camera = renderer->getCamera();
		EnableDepth();

		if (m_lights.size() == 0 && !warnNoLights) {
			warnNoLights = true;
			// app without 3d rendering doesn't care about this
			//log::out << log::YELLOW<<"Warning: rendering with no lights...\n";
		}
#ifdef ENGONE_PHYSICS
		std::unordered_map<MeshAsset*, std::vector<glm::mat4>> normalObjects;
		for (EngineWorld* world : info.app->m_worlds) {

			EngineObjectIterator iterator = world->createIterator();
			EngineObject* obj = 0;
			while (obj = iterator.next()) {

				ModelAsset* model = obj->getModel();
				rp3d::RigidBody* body = obj->getRigidBody();

				if (!model) continue;
				if (!model->valid()) continue;

				glm::mat4 modelMatrix = ToMatrix(body->getTransform());
				//Animator* animator = &obj->animator;

				// Get individual transforms
				std::vector<glm::mat4> transforms = model->getParentTransforms(nullptr);

				/*
				if ((transform->position.x-camPos.x)* (transform->position.x - camPos.x)
					+(transform->position.y - camPos.y)*(transform->position.y - camPos.y)+
					(transform->position.z - camPos.z)*(transform->position.z - camPos.z)>200*200) {
					continue;
				}
				*/
				// Draw instances
				for (uint32_t i = 0; i < model->instances.size(); ++i) {
					AssetInstance& instance = model->instances[i];
					//log::out << " " << instance.asset->filePath<< " " << (int)instance.asset->type << " "<< (int)asset->meshType << "\n";
					if (instance.asset->type != MeshAsset::TYPE) continue;

					MeshAsset* asset = (MeshAsset*)instance.asset;

					if (asset->meshType != MeshAsset::MeshType::Normal) continue;

					glm::mat4 out;
					out = modelMatrix * transforms[i] * instance.localMat;

					//out = glm::rotate(-20 * glm::pi<float>() / 180.f, glm::vec3(1, 0, 0));

					if (normalObjects.count(asset) > 0) {
						normalObjects[asset].push_back(out);
					} else {
						normalObjects[asset] = std::vector<glm::mat4>();
						normalObjects[asset].push_back(out);
					}
				}
			}
			//world->unlock();
			//world->deleteIterator(iterator);
		}
		//world->m_mutex.unlock();
		ShaderAsset* shader = assets->get<ShaderAsset>("object");
		if (!shader->getError()) {
			shader->bind();
			renderer->updatePerspective(shader);
			shader->setVec3("uCamera", camera->getPosition());

			bindLights(shader, { 0,0,0 });
			shader->setMat4("uTransform", glm::mat4(0)); // zero in mat4 means we do instanced rendering. uTransform should be ignored
			for (auto& [asset, vector] : normalObjects) {

				//log::out << "render " << asset->getLoadName()<< "\n";

				//if (asset->meshType == MeshAsset::MeshType::Normal) {
				for (uint32_t j = 0; j < asset->materials.size(); ++j) {// Maximum of 4 materials
					asset->materials[j]->bind(shader, j);
				}
				//}
				uint32_t remaining = vector.size();
				while (remaining > 0) {
					uint32_t batchAmount = std::min(remaining,CommonRenderer::INSTANCE_BATCH);
					
					renderer->instanceBuffer.setData(batchAmount * sizeof(glm::mat4), (vector.data() + vector.size()-remaining));

					asset->vertexArray.selectBuffer(3, &renderer->instanceBuffer);

					asset->vertexArray.draw(&asset->indexBuffer, batchAmount);
					remaining -= batchAmount;
				}
			}
		}
		shader = assets->get<ShaderAsset>("armature");
		if (!shader->getError()) {
			shader->bind();
			renderer->updatePerspective(shader);
			shader->setVec3("uCamera", camera->getPosition());
			for (EngineWorld* world : info.app->m_worlds) {
				//world->lock();

				EngineObjectIterator iterator = world->createIterator();
				EngineObject* obj;
				while (obj = iterator.next()) {

					ModelAsset* model = obj->getModel();
					rp3d::RigidBody* body = obj->getRigidBody();
					Animator* animator = obj->getAnimator();

					if (!model) continue;
					if (!model->valid()) continue;

					glm::mat4 modelMatrix = ToMatrix(body->getTransform());

					bindLights(shader, modelMatrix[3]);

					// Get individual transforms
					std::vector<glm::mat4> transforms;
					transforms = model->getParentTransforms(nullptr);

					if (transforms.size() != model->instances.size())
						continue;

					//-- Draw instances
					AssetInstance* armatureInstance = nullptr;
					ArmatureAsset* armatureAsset = nullptr;

					for (uint32_t i = 0; i < model->instances.size(); ++i) {
						AssetInstance& instance = model->instances[i];

						if (instance.asset->type == AssetArmature) {
							armatureInstance = &instance;
							armatureAsset = (ArmatureAsset*)instance.asset;
						} else if (instance.asset->type == AssetMesh) {
							if (!armatureInstance)
								continue;

							MeshAsset* meshAsset = (MeshAsset*)instance.asset;
							if (meshAsset->meshType != MeshAsset::MeshType::Boned) continue;
							if (instance.parent == -1) continue;

							std::vector<glm::mat4> boneMats;
							std::vector<glm::mat4> boneTransforms = model->getArmatureTransforms(animator, transforms[i], armatureInstance, armatureAsset, &boneMats);

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
				//world->unlock();
				//world->deleteIterator(iterator);
			}
		}
#endif
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
	void Engone::renderBasicDebugInfo(LoopInfo& info) {
		float sw = GetWidth();
		float sh = GetHeight();
		
		const int bufferSize = 50;
		char str[bufferSize];

		float rightEdge = sw - 3;
		float upEdge = 3;

		// should be changed
		FontAsset* consolas = info.window->getStorage()->get<FontAsset>("fonts/consolas42");

		auto& stats = info.app->getEngine()->getStats();
		float scroll = IsScrolledY();
		if (IsKeyDown(GLFW_KEY_LEFT_CONTROL)) {
			scroll *= 100;
		} else if (IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
			scroll *= 10;
		}

		const ui::Color highlighted = { 0.5f,0.8f,1.f,1.f };
		const ui::Color normalColor = { 1.f,1.0f,1.0f,1.f };

		std::string runStr = "Runtime ";
		runStr += FormatTime(stats.getRunTime(),true,FormatTimeH|FormatTimeM|FormatTimeS|FormatTimeMS);
		ui::TextBox runtimeBox = { runStr, 0,0,20,consolas,normalColor};
		runtimeBox.x = rightEdge - runtimeBox.getWidth();
		runtimeBox.y = upEdge;
		upEdge = runtimeBox.y + runtimeBox.h;

		std::string sleepStr = "Sleep time ";
		snprintf(str, bufferSize, "(%.1f%%) ", 100*stats.getSleepTime()/stats.getRunTime());
		sleepStr += str;
		sleepStr += FormatTime(stats.getSleepTime(),true, FormatTimeH | FormatTimeM | FormatTimeS | FormatTimeMS);
		ui::TextBox sleepBox = { sleepStr, 0,0,20,consolas,normalColor };
		sleepBox.x = rightEdge - sleepBox.getWidth();
		sleepBox.y = upEdge;
		upEdge = sleepBox.y + sleepBox.h;

		snprintf(str, bufferSize, "%.2f (%.2f) FPS", stats.getFPS(), stats.getFPSLimit());
		ui::TextBox fpsBox = { str,0,0,20,consolas,normalColor };
		fpsBox.x = rightEdge - fpsBox.getWidth();
		fpsBox.y = upEdge;
		upEdge = fpsBox.y + fpsBox.h;

		snprintf(str, bufferSize, "%.2f (%.2f) UPS", stats.getUPS(), stats.getUPSLimit());

		ui::TextBox upsBox = { str,0,0,20,consolas,normalColor };
		upsBox.x = rightEdge - upsBox.getWidth();
		upsBox.y = upEdge;
		upEdge = upsBox.y + upsBox.h;

		snprintf(str, bufferSize, "%.2f%% Game Speed", stats.getGameSpeed()*100);

		ui::TextBox speedBox = { str,0,0,20,consolas,normalColor };
		speedBox.x = rightEdge - speedBox.getWidth();
		speedBox.y = upEdge;
		upEdge = speedBox.y + speedBox.h;

		if (ui::Hover(fpsBox)) {
			fpsBox.rgba = highlighted;
			if (scroll != 0) {
				stats.setFPSLimit(stats.getFPSLimit() + scroll);
			}
		}
		if (ui::Hover(upsBox)) {
			upsBox.rgba = highlighted;
			if (scroll != 0) {
				stats.setUPSLimit(stats.getUPSLimit() + scroll);
			}
		}
		if (ui::Hover(speedBox)) {
			speedBox.rgba = highlighted;
			if (scroll != 0) {
				stats.setGameSpeed(stats.getGameSpeed() + scroll/100);
			}
		}

		const ui::Color areaColor = { 0,0.3 };
		float stoof[] = { fpsBox.getWidth() , upsBox.getWidth(), runtimeBox.getWidth(), sleepBox.getWidth(),speedBox.getWidth()};
		float areaW = 0;
		for (int i = 0; i < sizeof(stoof) / sizeof(float); i++) {
			if (stoof[i] > areaW) {
				areaW = stoof[i];
			}
		}
		float areaH = upsBox.y + upsBox.h;
		ui::Box area = { 0,0,areaW+6,areaH+6,areaColor };
		area.x = sw - area.w;
		area.y = 0;

		ui::Draw(area);
		ui::Draw(runtimeBox);
		ui::Draw(sleepBox);
		ui::Draw(fpsBox);
		ui::Draw(upsBox);
		ui::Draw(speedBox);
	}
	void Engone::addLight(Light* l) {
		m_lights.push_back(l);
	}
	LoopInfo& Engone::getLoopInfo() {
		return currentLoopInfo;
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