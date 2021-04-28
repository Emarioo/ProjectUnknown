#pragma once

#include "Rendering/Renderer.h"
#include "Managers/InterfaceManager.h"
#include "Sound/SoundManager.h"

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
	void UpdateUI(float delta);
	/*
	Update  objects, camera...
	*/
	void UpdateObjects(float delta);

	void RenderUI();
	/*
	Render objects, hitboxes...
	*/
	void RenderObjects();
	void RenderRawObjects();
	/*
	Start the game loop and give the engine update and render calls
	*/
	void Start(std::function<void(float)> update,std::function<void()> render);
}