#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Texture.h"
#include "Shader.h"
#include "../Components/Material.h"
#include "Light.h"
#include "../Objects/Camera.h"
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
		BorderlessFullscreen
	};

	class Renderer {
	private:

	public:

	};

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
	float GetMouseX();
	float GetMouseY();
	float GetFloatMouseX();
	float GetFloatMouseY();
	bool HasFocus();

	void SetInterfaceCallbacks(
		std::function<void(int, int)> key,
		std::function<void(double, double, int, int)> mouse,
		std::function<void(double)> scroll,
		std::function<void(double, double)> drag,
		std::function<void(int, int)> resize,
		std::function<void(int)> focus);

	/*
	Make sure the windowType isn't none
	*/
	void MakeWindow(const char* title);
	void SetWindowType(WindowTypes t);

	void UpdateViewMatrix(double lag);

	bool IsCursorVisible();
	void SetCursorVisibility(bool f);
	/*
	Also known as gameplay/first person mode.
	*/
	bool IsCursorLocked();
	/*
	Disable cursor and use raw cursor motion. Gameplay/first person mode in other words.
	*/
	void LockCursor(bool f);

	/*
	Starts glfw, projection matrix, shaders, rect and text container
	*/
	void InitRenderer();
	void RenderClearScreen(float r, float g, float b, float a);
	bool RenderRunning();
	void RenderTermin();

	void SetProjection(float ratio);
	void SwitchBlendDepth(bool b);

	bool BindShader(unsigned char shader);

	Shader* GetShader(unsigned char shader);
	Shader* GetBoundShader();

	void AddShader(unsigned char shader, const std::string& path);
	void AddShaderFromInclude(unsigned char shader, const std::string& text);

	/*
	Set transform uniform for current shader
	*/
	void SetTransform(glm::mat4 m);
	/*
	Set projection matrix uniform for current shader
	*/
	void UpdateProjection();
	/*
	Set color uniform for current shader
	*/
	void SetColor(float r, float g, float b, float a);

	/*
	Set transform uniform for interface shader
	*/
	void SetTransform(float x, float y);
	/*
	Set size uniform for current shader
	*/
	void SetSize(float w, float h);
	/*
	Set render area uniform for interface shader
	*/
	void SetRenderArea(float f0, float f1, float f2, float f3);

	void BindTexture(int slot, const std::string& name);
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