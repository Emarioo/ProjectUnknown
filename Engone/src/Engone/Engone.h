#pragma once

#include "Rendering/Renderer.h"
#include "Handlers/ObjectHandler.h"
#include "UI/GuiHandler.h"
#include "Sound/SoundHandler.h"
#include "DebugHandler.h"
#include "Utility/Utilities.h"
#include "GameState.h"
#include "Server/NetworkHandler.h"

#include "Module.h"

/*
Include this file in your Application.cpp to get access to all the engine's functions. But not really haha

Your own update function should include the UpdateObjects, UpdateCamera and UpdateUI functions.
Your render function should include RenderObjects, RenderUI and RenderRawObjects incase of specialties.
*/
namespace engone {

	class Engone : public Module {
	public:
		Engone() = default;
		~Engone();

		void Init() override;
		void Update(float delta) override;
		void Render() override;



		std::vector<Module*> modules;
	};
	
	FrameBuffer& GetDepthBuffer();
	glm::mat4& GetLightProj();
	void SetProjection(float ratio);
	void UpdateProjection(Shader* shader);

	/*
	Initialize engine, window, glfw event listeners...
	*/
	void InitEngine();
	void ExitEngine();

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

	struct Timer
	{
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

	/*
	Set light[index] uniform to currentShader
	*/
	void PassLight(Shader* shader, DirLight* light);
	void PassLight(Shader* shader, int index, PointLight* light);
	void PassLight(Shader* shader, int index, SpotLight* light);
	void PassMaterial(Shader* shader, int index, MaterialAsset* material);


	/*
	Start the game loop and give the engine update and render calls.
	The argument is delta for update and lag for render
	*/
	void Start(std::function<void(double)> update,std::function<void(double)> render, double fps);
}