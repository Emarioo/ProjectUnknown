#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

#include "Texture.h"
#include "Shader.h"
#include "Managers/FileManager.h"
#include "Rendering/Light.h"
#include "Camera.h"
#include "UI/Font.h"
#include "FrameBuffer.h"


// This class is the renderer class
// It handles textures, lights and shaders
// It does not control which objects to render but is instead used as a tool to render them

namespace engine {

	enum WindowTypes {
		Minimized,
		WindowBorderless,
		WindowBorder,
		Maximized,
		Fullscreen
	};

	float Wid();
	float Hei();
	float Width();
	float Height();
	void SetWinSize(int x, int y, int w, int h);
	void SetWindowType(WindowTypes t);
	WindowTypes GetWindowType();
	WindowTypes GetLastWindowType();
	float AlterW(float w);
	float AlterH(float h);
	float AlterSW(float w);
	float AlterSH(float h);
	float AlterX(int x);
	float AlterY(int y);

	bool HasFocus();

	Camera* GetCamera();
	/*
	Bind Shader before
	*/
	void UpdateViewMatrix();

	bool GetCursorMode();
	/*
	Activate or deactivate cursor
	*/
	void SetCursorMode(bool f);
	double GetMX();
	double GetMY();
	GLFWwindow* GetWindow();
	/*
	Starts glfw, projection matrix, rect and text container
	*/
	void InitRenderer();
	void RenderClearScreen(float r, float g, float b, float a);
	bool RenderRunning();
	void RenderTermin();
	bool IsFullscreen();
	bool IsMaximized();
	bool IsMinimized();
	void SetProjection(float ratio);
	void SwitchBlendDepth(bool b);
	void SetWindowTitle(const char*);

	void SetInterfaceCallbacks(
		std::function<void(int, int)> key,
		std::function<void(double, double, int, int)> mouse,
		std::function<void(double)> scroll,
		std::function<void(double, double)> drag,
		std::function<void(int, int)> resize,
		std::function<void(int)> focus);

	//void RenderBorder();

	//void RenderScene();

	// Shader
	bool BindShader(ShaderType type);
	/*
	Can return nullptr if materia errorerd
	*/
	Shader* GetShader(ShaderType type);
	/*
	Returns nullptr if error?
	*/
	Shader* GetShader();

	void AddShader(ShaderType, const std::string& name);
	/*
	Bind Shader before
	*/
	void PassTransform(glm::mat4 m);
	/*
	Bind Shader before
	*/
	void PassProjection();
	void PassColor(float r, float g, float b, float a);
	void GuiTransform(float x, float y);
	void GuiSize(float w, float h);
	void GuiColor(float r, float g, float b, float a);

	// Texture
	void BindTexture(int slot, const std::string& name);
	void AddTextureAsset(const std::string& name);

	// Render
	/*
	Closest Lights need to be bound before drawing mesh
	*/
	
	// Global Character Limit
	/*
	x, y - is position of text
	centerX/Y - center x,y or not
	Call GuiColor and GuiTransform (and bind shader) before calling this function. Font is bound in the function
	atChar - position of the marker (-1 for no marker)
	*/
	void DrawString(Font *font, const std::string&,bool center,float height,int atChar);
	/*
	Call GuiColor, GuiSize and GuiTransform (and bind shader) before calling this function.
	*/
	void DrawRect();
	void DrawRect(float x,float y);
	void DrawRect(float x,float y,float w,float h);
	void DrawRect(float x,float y,float w,float h,float r,float g,float b,float a);

	// Light
	void AddLight(Light* l);
	void DelLight(Light* l);
	void BindLights(glm::vec3 objectPos);
	std::vector<Light*>& GetLights();

	/*
	Set light[index] uniform to currentShader
	*/
	void PassLight(DirLight* light);
	void PassLight(int index, PointLight* light);
	void PassLight(int index, SpotLight* light);
	void PassMaterial(Material& material);
}