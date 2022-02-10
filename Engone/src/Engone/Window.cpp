#include "gonpch.h"

#include "Rendering/Renderer.h"

#include "Window.h"

namespace engone {

	static WindowType windowType=WindowType::Windowed;
	static GLFWwindow* glfwWindow=nullptr;
	GLFWwindow* GetWindow() {
		return glfwWindow;
	}
	static std::string title = "Unnamed";

	// relevant if windowed, stores the coordinates when going from windowed->fullscreen->windowed
	static int x=0, y=0, w=0, h=0;

	static bool isCursorVisible = true, isCursorLocked = false, hasFocus = true;

	void InitWindow(const std::string& title) {
		// Temporary, should be loaded from a settings file
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			x = mode->width / 6;
			y = mode->height / 6;
			w = mode->width / 1.5;
			h = mode->height / 1.5;
			windowType = WindowType::Windowed;
		}
		MakeWindow(windowType);

		SetTitle(title);
	}
	void SetTitle(const std::string& title) {
		glfwSetWindowTitle(glfwWindow, title.c_str());
	}
	void MakeWindow(WindowType type) {
		windowType = type;
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		if (glfwWindow != nullptr) {
			glfwDestroyWindow(glfwWindow);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		if (type == WindowType::Windowed) {
			glfwWindowHint(GLFW_DECORATED, true);
			glfwWindow = glfwCreateWindow(w, h, "Unnamed", NULL, NULL);

		}
		else if (type == WindowType::Fullscreen) {
			glfwWindowHint(GLFW_DECORATED, true);
			glfwWindow = glfwCreateWindow(mode->width, mode->height, "Unnamed", monitor, NULL);

		}
		else if (type == WindowType::BorderlessFullscreen) {
			glfwWindowHint(GLFW_DECORATED, false);
			glfwWindow = glfwCreateWindow(mode->width, mode->height, "Unnamed", NULL, NULL);
		}
		else {
			std::cout << "WindowType is None" << std::endl;
			return;
		}
		if (!glfwWindow) {
			glfwTerminate();
			std::cout << "Terminate Window!" << std::endl;
			return;
		}
		glfwMakeContextCurrent(glfwWindow);
	}
	void SetType(WindowType type) {
		if (type == windowType) return;

		if (glfwWindow == nullptr) {
			windowType = type;
			return;
		}
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		if (windowType == WindowType::Windowed && type == WindowType::Fullscreen) {
			glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else if (windowType == WindowType::Windowed && type == WindowType::BorderlessFullscreen) {
			MakeWindow(type);
		}
		else if (windowType == WindowType::Fullscreen && type == WindowType::Windowed) {
			glfwSetWindowMonitor(glfwWindow, NULL, x, y, w, h, mode->refreshRate);
		}
		else if (windowType == WindowType::Fullscreen && type == WindowType::BorderlessFullscreen) {
			MakeWindow(type);
		}
		else if (windowType == WindowType::BorderlessFullscreen && type == WindowType::Windowed) {
			MakeWindow(type);
		}
		else if (windowType == WindowType::BorderlessFullscreen && type == WindowType::Fullscreen) {
			MakeWindow(type);
		}
		windowType = type;
		int wid;
		int hei;
		glfwGetWindowSize(glfwWindow, &wid, &hei);
		glViewport(0, 0, wid, hei);
	}
	WindowType GetType() {
		return windowType;
	}
	float GetWidth() {
		int temp;
		glfwGetWindowSize(glfwWindow, &temp, nullptr);
		return temp;
	}
	float GetHeight() {
		int temp;
		glfwGetWindowSize(glfwWindow, nullptr, &temp);
		return temp;
	}
	bool HasFocus() {
		return hasFocus;
	}
	bool IsCursorVisible() {
		return isCursorVisible;
	}
	bool IsCursorLocked() {
		return isCursorLocked;
	}
	void SetCursorVisible(bool visible) {
		isCursorVisible = visible;
		if (visible) glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	void LockCursor(bool locked) {
		isCursorLocked = locked;
		if (locked) {
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
		else {
			SetCursorVisible(isCursorVisible);
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}
	}
	bool IsOpen() {
		return !glfwWindowShouldClose(glfwWindow);
	}
}