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
#include "../Components/Material.h"
#include "Light.h"
#include "Camera.h"
#include "../UI/Font.h"
#include "FrameBuffer.h"


// This class is the renderer class. Not really a class but you get the idea...
// It handles textures, lights and shaders
// It does not control which objects to render but is instead used as a tool to render them

namespace engine {

	enum WindowTypes {
		NONE,
		Windowed,
		Fullscreen,
		FullscreenBorderless,
		//WindowedBorderless,
	};
	void SetWindowType(WindowTypes t);
	WindowTypes GetWindowType();
	GLFWwindow* GetWindow();
	void SetWindowTitle(const char*);
	void SetWindowPos(int x, int y);
	void SetWindowSize(int w, int h);

	float Width();
	float Height();
	/*
	From pixel to screen range (-1 to 1)
	*/
	float ToFloatScreenX(int x);
	/*
	From pixel to screen range (-1 to 1)
	*/
	float ToFloatScreenY(int y);
	/*
	From pixel to screen range (-1 to 1)
	*/
	float ToFloatScreenW(float w);
	/*
	From pixel to screen range (-1 to 1)
	*/
	float ToFloatScreenH(float h);
	
	bool HasFocus();

	Camera* GetCamera();
	/*
	Bind Shader before
	*/
	void UpdateViewMatrix();

	bool IsCursorVisible();
	void SetCursorVisibility(bool f);
	/*
	Also known as is cursor in gameplay mode.
	*/
	bool IsCursorLocked();
	/*
	Disable cursor and use raw cursor motion. Gameplay mode in other words.
	*/
	void LockCursor(bool f);

	float GetMouseX();
	float GetMouseY();
	float GetFloatMouseX();
	float GetFloatMouseY();
	/*
	Starts glfw, projection matrix, shaders, rect and text container
	*/
	void InitRenderer();
	void RenderClearScreen(float r, float g, float b, float a);
	bool RenderRunning();
	void RenderTermin();

	void SetProjection(float ratio);
	void SwitchBlendDepth(bool b);

	void SetInterfaceCallbacks(
		std::function<void(int, int)> key,
		std::function<void(double, double, int, int)> mouse,
		std::function<void(double)> scroll,
		std::function<void(double, double)> drag,
		std::function<void(int, int)> resize,
		std::function<void(int)> focus);

	//void RenderBorder();

	//void RenderScene();

	bool BindShader(unsigned char shader);
	/*
	Can return nullptr if materia caused an error
	*/
	Shader* GetShader(unsigned char shader);
	/*
	Returns none shader if error?
	*/
	Shader* GetBoundShader();

	void AddShader(unsigned char shader, const std::string& path);

	/*
	Set uniform for current shader
	*/
	void SetTransform(glm::mat4 m);
	/*
	Set uniform for current shader
	*/
	void SetProjection();
	/*
	Set uniform for current shader
	*/
	void SetColor(float r, float g, float b, float a);

	/*
	Set uniform for interface shader
	*/
	void SetTransform(float x, float y);
	/*
	Set uniform for current shader
	*/
	void SetSize(float w, float h);
	/*
	Set uniform for interface shader
	*/
	void SetRenderArea(float f0, float f1, float f2, float f3);

	// Texture
	void BindTexture(int slot, const std::string& name);
	// Texture
	void BindTexture(int slot, Texture* texture);
	
	void AddFont(const std::string& name);
	Font* GetFont(const std::string& name);

	// Render
	/*
	Closest Lights need to be bound before drawing mesh
	*/
	
	// Global Character Limit
	/*
	GuiTransform for position
	GuiColor for color
	GuiSize for scaling
	atChar is the position for the marker (-1 for no marker)
	text size will be adapted depending on height
	*/
	void DrawString(const std::string& font, const std::string& text,bool center,float height,int atChar);
	/*
	GuiTransform for position
	GuiColor for color
	GuiSize for scaling
	atChar is the position for the marker (-1 for no marker)
	text size will be adapted depending on width and height
	*/
	void DrawString(const std::string& font, const std::string& text,bool center,float width,float height,int atChar);
	/*
	Call GuiColor, GuiSize and GuiTransform (and bind shader) before calling this function.
	*/
	void DrawRect();
	void DrawRect(float x,float y);
	void DrawRect(float x,float y,float w,float h);
	void DrawRect(float x,float y,float w,float h,float r,float g,float b,float a);
	/*
	Calls GuiSize(1,1), GuiTransform(1, 1) and also GuiColor(1,1,1,1)
	*/
	void DrawUVRect(float x, float y, float xw, float yh, float u, float v, float uw, float vh);

	// Light
	void AddLight(Light* l);
	void RemoveLight(Light* l);
	void BindLights(glm::vec3 objectPos);
	std::vector<Light*>& GetLights();

	/*
	Set light[index] uniform to currentShader
	*/
	void PassLight(DirLight* light);
	void PassLight(int index, PointLight* light);
	void PassLight(int index, SpotLight* light);
	void PassMaterial(int index, Material* material);
}