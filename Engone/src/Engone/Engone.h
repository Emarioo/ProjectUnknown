#pragma once

#include "Rendering/Renderer.h"
#include "Window.h"

//#include "UI/GuiHandler.h"
//#include "UI/UIPipeline.h"
#include "Sound/SoundHandler.h"
#include "Utility/Utilities.h"
#include "GameState.h"
#include "Server/NetworkHandler.h"

#include "Components/EntitySystem.h"

#include "Rendering/FrameBuffer.h"
#include "Rendering/Light.h"

#include "Window.h"

#include "Objects/Camera.h"

/*
Include this file in your Application.cpp to get access to all the engine's functions. But not really haha

Your own update function should include the UpdateObjects, UpdateCamera and UpdateUI functions.
Your render function should include RenderObjects, RenderUI and RenderRawObjects incase of specialties.
*/
namespace engone {

	// Init glfw, glew, guiShader. has ui functions
	void InitEngone(EngoneHint hints=EngoneHint::None);
	void UninitEngone();

	/*
	Start the game loop and give the engine update and render calls.
	The argument is delta for update and lag for render
	*/
	void Start(std::function<void(double)> update, std::function<void(double)> render, double fps);

	FrameBuffer& GetDepthBuffer();

	//Camera* GetCamera();

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

	struct Delayed {
		float time;
		std::function<void()> func;
	};

	void AddTimer(float time, std::function<void()> func);
	void UpdateTimers(float delta);

	// Light
	void AddLight(Light* l);
	void RemoveLight(Light* l);

	/*
	Binds light to current shader
	 If one of the four closest light are already bound then don't rebind them [Not added]
	*/
	void BindLights(Shader* shader, glm::vec3 objectPos);
	std::vector<Light*>& GetLights();
}