#pragma once

#include "Rendering/Renderer.h"
#include "Handlers/ObjectHandler.h"
#include "Handlers/UIHandler.h"
#include "Handlers/SoundHandler.h"
#include "DebugTool/DebugHandler.h"
#include "GameState.h"

/*
Include this file in your Application.cpp to get access to all the engine's functions
*/

namespace engine {
	
	FrameBuffer& GetDepthBuffer();
	glm::mat4& GetLightProj();
	/*
	Initialize engine, window, event...
	 Use GetEventCallbacks if you need to handle mouse and key events
	*/
	void Initialize();
	void Uninitialize();

	/*
	Update IElements, Timed Functions
	*/
	void UpdateUI(double delta);
	/*
	Update  objects, camera...
	*/
	void UpdateObjects(double delta);

	void RenderUI(double delta);
	/*
	Render objects, hitboxes...
	*/
	void RenderObjects(double delta);
	void RenderRawObjects(double delta);
	/*
	Start the game loop and give the engine update and render calls
	*/
	void Start(std::function<void(double)> update,std::function<void(double)> render);
}