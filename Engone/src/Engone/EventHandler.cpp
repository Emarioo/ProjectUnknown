#include "gonpch.h"

#include "EventHandler.h"

#include "Rendering/Renderer.h"

namespace engone
{
	EventHandler* eventHandler;
	void EventHandler::AddListener(EventListener* listener)
	{
		listeners.push_back(listener);
	}
	int EventHandler::GetMouseX()
	{
		return mouseX;
	}
	int EventHandler::GetMouseY()
	{
		return mouseY;
	}
	void EventHandler::SetMouseX(int x)
	{
		mouseX = x;
	}
	void EventHandler::SetMouseY(int y)
	{
		mouseY = y;
	}
	void EventHandler::AddEvent(Event* e)
	{
		events.push_back(e);
	}
	void EventHandler::DispatchEvents()
	{
		for (int i = 0; i < listeners.size();i++) {
			for (int j = 0; j < events.size();j++) {
				listeners[i]->run(events[i]);
			}
		}
		while (events.size() > 0)
			events.pop_back();
	}
	EventListener::EventListener()
	{
		eventHandler->AddListener(this);
	}
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		// This is for debug purposes
		if (key == GLFW_KEY_1) {
			
			renderer->SetWindowType(Windowed);
		}
		else if (key == GLFW_KEY_2) {
			renderer->SetWindowType(Fullscreen);
		}
		else if (key == GLFW_KEY_3) {
			renderer->SetWindowType(BorderlessFullscreen);
		}

		eventHandler->AddEvent(new Event(EventType::Key, key,action));

		//if (KeyEvent != nullptr) KeyEvent(key, action);
		
	}
	void MouseCallback(GLFWwindow* window, int action, int button, int mods) {
		eventHandler->AddEvent(new Event(EventType::Mouse, eventHandler->GetMouseX(), eventHandler->GetMouseY(), action, button));
		//if (MouseEvent != nullptr) MouseEvent(renMouseX, renMouseY, action, button);
	}
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		eventHandler->AddEvent(new Event(EventType::Scroll, yoffset));
		//if (ScrollEvent != nullptr)  ScrollEvent(yoffset);
		
	}
	void DragCallback(GLFWwindow* window, double mx, double my) {
		eventHandler->AddEvent(new Event(EventType::Drag, eventHandler->GetMouseX(), eventHandler->GetMouseY()));
		/*if (IsCursorLocked()) {
			GetCamera()->rotation.y -= (mx - renMouseX) * (3.14159f / 360) * cameraSensitivity;
			GetCamera()->rotation.x -= (my - renMouseY) * (3.14159f / 360) * cameraSensitivity;
		}
		if (DragEvent != nullptr)
			DragEvent(mx, my);
		renMouseX = mx;
		renMouseY = my;*/
	}
	void ResizeCallback(GLFWwindow* window, int width, int height) {
		eventHandler->AddEvent(new Event(EventType::Resize, width, height));
		/*glViewport(0, 0, width, height);
		if (windowType == Windowed) {
			winW = width;
			winH = height;
		}
		if (ResizeEvent != nullptr)
			ResizeEvent(width, height);*/
	}
	void WindowFocusCallback(GLFWwindow* window, int focus) {
		eventHandler->AddEvent(new Event(EventType::Focus, focus));
		/*windowHasFocus = focus;
		if (FocusEvent != nullptr) {
			FocusEvent(focus);
		}*/
	}
	void EventHandler::Init()
	{
		glfwSetKeyCallback(renderer->GetWindow(), KeyCallback);
		glfwSetMouseButtonCallback(renderer->GetWindow(), MouseCallback);
		glfwSetScrollCallback(renderer->GetWindow(), ScrollCallback);
		glfwSetCursorPosCallback(renderer->GetWindow(), DragCallback);
		glfwSetWindowFocusCallback(renderer->GetWindow(), WindowFocusCallback);
		glfwSetWindowSizeCallback(renderer->GetWindow(), ResizeCallback);
	}
}