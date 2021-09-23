#pragma once

#include "Rendering/Renderer.h"
#include "Handlers/ObjectHandler.h"
#include "Handlers/UIHandler.h"
#include "Handlers/SoundHandler.h"
#include "DebugTool/DebugHandler.h"
#include "Utility/Utilities.h"
#include "GameState.h"

/*
Include this file in your Application.cpp to get access to all the engine's functions. But not really haha

Your own update function should include the UpdateObjects, UpdateCamera and UpdateUI functions.
Your render function should include RenderObjects, RenderUI and RenderRawObjects incase of specialties.
*/
namespace engone {
	
	FrameBuffer& GetDepthBuffer();
	glm::mat4& GetLightProj();

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

	void RenderUI(double lag);
	/*
	Render objects, hitboxes with the appropriate shaders.
	*/
	void RenderObjects(double lag);
	/*
	Render objects without doing anything but calling the object's draw function.
	*/
	void RenderRawObjects(double lag);
	/*
	Start the game loop and give the engine update and render calls.
	The argument is delta for update and lag for render
	*/
	void Start(std::function<void(double)> update,std::function<void(double)> render, double fps);
}