#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Managers/FileManager.h"
#include "Rendering/Light.h"
#include "Camera.h"
#include "UI/Font.h"

// This class is the renderer class
// It handles textures, lights and shaders
// It does not control which objects to render but is instead used as a tool to render them

namespace renderer {

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
	void UpdateProj();

	bool GetCursorMode();
	/*
	Activated or deactivate cursor
	*/
	void SetCursorMode(bool f);
	double GetMX();
	double GetMY();
	GLFWwindow* GetWindow();
	/*
	Starts glfw, shaders and base projection matrix
	*/
	void Init();
	void RenderClearScreen(float r, float g, float b, float a);
	bool RenderRunning();
	void RenderTermin();
	bool IsFullscreen();
	bool IsMaximized();
	bool IsMinimized();
	void SetProjection(float ratio);
	void SwitchBlendDepth(bool b);
	void SetWindowTitle(const char*);

	void SetCallbacks(
		void(*key)(int, int),
		void(*mouse)(double, double, int, int),
		void(*scroll)(double, double),
		void(*drag)(double, double),
		void(*resize)(int, int),
		void(*focus)(int));

	//void RenderBorder();

	//void RenderScene();

	// Shader
	void BindShader(MaterialType type);
	/*
	Can return nullptr if materia errorerd
	*/
	Shader* GetShader(MaterialType type);
	/*
	Bind Shader before
	*/
	void ObjectTransform(glm::mat4 m);
	/*
	Bind Shader before
	*/
	void ObjectProjection(glm::mat4 m);
	void ObjectColor(float r, float g, float b, float a);
	void GuiTransform(float x, float y);
	void GuiSize(float w, float h);
	void GuiColor(float r, float g, float b, float a);

	// Texture
	void BindTexture(const std::string& name);
	void AddTexture(const std::string& name, std::string path);

	// Render
	/*
	Closest Lights need to be bound before drawing mesh
	*/
	void DrawMesh(MeshData* meshData, glm::mat4 trans);
	
	// Global Character Limit
	/*
	x, y - is position of text
	centerX/Y - center x,y or not
	Call GuiColor and GuiTransform (and bind shader) before calling this function. Font is bound in the function
	atChar - position of the marker (-1 for no marker)
	*/
	void DrawText(Font *font, const std::string&,bool center,float height,int atChar);
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
	void BindLights(glm::vec3 pos);
}