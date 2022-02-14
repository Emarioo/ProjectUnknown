#pragma once

#include "Rendering/Renderer.h"
#include "Window.h"

#include "Handlers/ObjectHandler.h"
#include "UI/GuiHandler.h"
#include "Sound/SoundHandler.h"
#include "Utility/Utilities.h"
#include "GameState.h"
#include "Server/NetworkHandler.h"

#include "Rendering/FrameBuffer.h"
#include "Rendering/Light.h"

#include "Window.h"

/*
Include this file in your Application.cpp to get access to all the engine's functions. But not really haha

Your own update function should include the UpdateObjects, UpdateCamera and UpdateUI functions.
Your render function should include RenderObjects, RenderUI and RenderRawObjects incase of specialties.
*/
namespace engone {

	// Init glfw, glew, guiShader. has ui functions
	void InitEngone();
	void UninitEngone();

	/*
	Start the game loop and give the engine update and render calls.
	The argument is delta for update and lag for render
	*/
	void Start(std::function<void(double)> update, std::function<void(double)> render, double fps);

	/*
	Window window;
	std::vector<Module*> modules;
	// Module needs a unique id
	void AddModule(Module* module);
	template<class T>
	T* GetModule() {
		for (int i = 0; i < modules.size();i++) {
			if (modules[i].id == T::ID) {
				return reinterpret_cast<T*>(modules[i]);
			}
		}
		return nullptr;
	}
	*/

	FrameBuffer& GetDepthBuffer();
	glm::mat4& GetLightProj();
	void SetProjection(float ratio);
	void UpdateProjection(Shader* shader);

	/*
	Update objects and camera (viewMatrix)
	 also dimensions but they should be moved somewhere else.
	*/
	void UpdateObjects(float delta);

	//void RenderUI(double lag);
	/*
	Render objects, hitboxes with the appropriate shaders.
	*/
	void RenderObjects(double lag);
	/*
	Render objects without doing anything but calling the object's draw function.
	*/
	void RenderRawObjects(Shader* shader, double lag);

	/*
	Render objects, ui, debug tool
	*/
	void RenderEngine(double lag);
	/*
	Update objects, ui, debug tool
	*/
	void UpdateEngine(double delta);

	struct Timer {
		float time;
		std::function<void()> func;
	};

	void AddTimer(float time, std::function<void()> func);
	void UpdateTimers(float delta);

	// Light
	void AddLight(Light* l);
	void RemoveLight(Light* l);

	void BindLights(Shader* shader, glm::vec3 objectPos);
	std::vector<Light*>& GetLights();
}