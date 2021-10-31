#pragma once

#include "Rendering/Renderer.h"
#include "Handlers/ObjectHandler.h"
#include "UI/GUIManager.h"
#include "Handlers/SoundHandler.h"
#include "DebugTool.h"
#include "Utility/Utilities.h"
#include "GameState.h"
#include "Server/NetworkManager.h"

/*
Include this file in your Application.cpp to get access to all the engine's functions. But not really haha

Your own update function should include the UpdateObjects, UpdateCamera and UpdateUI functions.
Your render function should include RenderObjects, RenderUI and RenderRawObjects incase of specialties.
*/
namespace engone {
	
	FrameBuffer& GetDepthBuffer();
	glm::mat4& GetLightProj();
	void SetProjection(float ratio);
	void UpdateProjection(Shader* shader);

	/*
	Initialize engine, window, event...
	 Use GetEventCallbacks if you need to handle mouse and key events
	*/
	void InitEngone();
	void ExitEngone();

	/*
	Update IElements, Timed Functions
	*/
	void UpdateUI(double delta);
	/*
	Update objects and camera (viewMatrix)
	 also dimensions but they should be moved somewhere else.
	*/
	void UpdateObjects(double delta);

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

	struct TimedFunc
	{
		TimedFunc(std::function<void()> f, float t) : func(f), time(t) {}
		std::function<void()> func;
		float time = 0;
	};

	void AddTimedFunction(std::function<void()> func, float time);
	void UpdateTimedFunc(float delta);

	/*
	Start the game loop and give the engine update and render calls.
	The argument is delta for update and lag for render
	*/
	void Start(std::function<void(double)> update,std::function<void(double)> render, double fps);
}