#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <map>

#include "Objects/GameObject.h"
#include "Gui/IInput.h"
#include "World/Dimension.h"

#include "Utility/Keyboard.h"
#include "Chat.h"

void InitVisual();

void AddRenderO(std::string,GameObject*);
void DelRenderO(std::string,GameObject*);
void ClearRender(std::string);
void DrawVisual();
void SetDimension(Dimension*);
void UpdateVisual();

//void WindowFocusCallback(GLFWwindow* window, int focus);

//void SetEscMode(bool f);
bool GetPauseMode();
bool GetCursorMode();

bool GetChatMode();
void UpdateChat();
/*
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MouseCallback(GLFWwindow* window, int button, int action, int mods);
void CursorCallback(GLFWwindow* window, double mx, double my);
*/