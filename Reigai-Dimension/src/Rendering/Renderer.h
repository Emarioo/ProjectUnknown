#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <map>
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Components/Mesh.h"
#include "Utility/FileHandler.h"
#include "Rendering/Light.h"
#include "Camera.h"

#include "Data/DataManager.h"

// This class is the renderer class
// It handles textures, lights and shaders
// It does not control which objects to render but is instead used as a tool to render them

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
void SetWinSize(int x,int y,int w, int h);
void SetWindowType(WindowTypes t);
WindowTypes GetWindowType();
WindowTypes GetLastWindowType();
float AlterW(float w);
float AlterH(float h);
float AlterX(float x);
float AlterY(float y);

bool HasFocus();

Camera* GetCamera();
void UpdateProj();

bool GetCursorMode();
/*
Activated or deactivate cursor
*/
void SetCursorMode(bool f);

GLFWwindow* GetWindow();
void RenderInit();
void RenderClearScreen(float r,float g,float b,float a);
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
	void(*mouse)(double,double,int, int),
	void(*drag)(double, double),
	void(*resize)(int, int),
	void(*focus)(int));

//void RenderBorder();

//void RenderScene();

// Shader
void BindShader(std::string name);
void AddShader(std::string name, std::string path);
/*
Can return nullptr
*/
Shader* GetShader(std::string name);
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
void GuiColor(float r, float g,float b,float a);

// Texture
void BindTexture(std::string name);
void AddTexture(std::string name, std::string path);

// Render
/*
Closest Lights need to be bound before drawing mesh
*/
void DrawMesh(std::string name, glm::mat4 trans);

// Light
void AddLight(Light* l);
void DelLight(Light* l);
void BindLights(std::string s, glm::vec3 pos);
