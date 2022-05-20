#pragma once

#include "Engone/Rendering/Renderer.h"
#include "Engone/Window.h"

//#include "UI/GuiHandler.h"
//#include "UI/UIPipeline.h"
#include "Engone/SoundModule.h"
#include "Engone/Utility/Utilities.h"
#include "Engone/GameState.h"
#include "Engone/NetworkModule.h"

#include "Engone/Components/EntitySystem.h"

#include "Engone/Rendering/FrameBuffer.h"
#include "Engone/Rendering/Light.h"

#include "Engone/Window.h"

#include "Engone/Objects/Camera.h"
#include "Engone/Application.h"

/*
Include this file in your main.cpp to get access to all the engine's functions. But not really haha

Your own update function should include the UpdateObjects, UpdateCamera and UpdateUI functions.
Your render function should include RenderObjects, RenderUI and RenderRawObjects incase of specialties.
*/
namespace engone {

	// Currently only sets options
	void Initialize(EngoneOption hints=EngoneOption::None);
	// Not really done
	void Cleanup();

	/*
	Start the loop running the applications.
	When all windows of all applications are closed the loop will end.
	*/
	void Start();
	void AddApplication(Application* app);

	FrameBuffer& GetDepthBuffer();

	//Camera* GetCamera();

	/*
	Update objects and camera (viewMatrix)
	 also dimensions but they should be moved somewhere else.
	*/
	void UpdateObjects(float delta);

	/*
	Render objects, hitboxes with the appropriate shaders.
	*/
	void RenderObjects(float lag);
	/*
	Render objects without doing anything but calling the object's draw function.
	*/
	void RenderRawObjects(Shader* shader, float lag);

	/*
	Render objects, ui, debug tool
	*/
	void RenderEngine(float lag);
	/*
	Update objects, ui, debug tool
	*/
	void UpdateEngine(float delta);

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