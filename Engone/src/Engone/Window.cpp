#include "gonpch.h"

#include "Window.h"

#include "Rendering/Renderer.h"

namespace engone {
	Window::Window() {

	}
	Window::Window(const std::string& title) {
		SetTitle(title);

		Init();
	}
	Window::~Window() {

	}
	void Window::Init() {
		// Temporary, should be loaded from a settings file
		windowType = WindowType::Windowed;
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		x = mode->width / 6;
		y = mode->height / 6;
		w = mode->width / 1.5;
		h = mode->height / 1.5;

		MakeWindow();
	}
	void Window::Update(float delta) {

	}
	void Window::Render() {

	}
	void Window::SetTitle(const std::string& title) {
		glfwSetWindowTitle(glfwWindow, title.c_str());
	}
	void Window::MakeWindow() {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		if (glfwWindow != nullptr) {
			glfwDestroyWindow(glfwWindow);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		if (windowType == WindowType::Windowed) {
			glfwWindowHint(GLFW_DECORATED, true);
			glfwWindow = glfwCreateWindow(w, h, "Unnamed", NULL, NULL);

		}
		else if (windowType == WindowType::Fullscreen) {
			glfwWindowHint(GLFW_DECORATED, true);
			glfwWindow = glfwCreateWindow(mode->width, mode->height, "Unnamed", monitor, NULL);

		}
		else if (windowType == WindowType::BorderlessFullscreen) {
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
	void Window::SetType(WindowType type) {
		if (type == windowType) return;

		if (glfwWindow == nullptr) {
			windowType = type;
			return;
		}
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		if (windowType == WindowType::Windowed && type == WindowType::Fullscreen) {
			windowType = type;// do it before so the resize event has the current windowType and not the last one
			glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else if (windowType == WindowType::Windowed && type == WindowType::BorderlessFullscreen) {
			windowType = type;
			MakeWindow();
		}
		else if (windowType == WindowType::Fullscreen && type == WindowType::Windowed) {
			windowType = type;
			glfwSetWindowMonitor(glfwWindow, NULL, x, y, w, h, mode->refreshRate);
		}
		else if (windowType == WindowType::Fullscreen && type == WindowType::BorderlessFullscreen) {
			windowType = type;
			MakeWindow();
		}
		else if (windowType == WindowType::BorderlessFullscreen && type == WindowType::Windowed) {
			windowType = type;
			MakeWindow();
		}
		else if (windowType == WindowType::BorderlessFullscreen && type == WindowType::Fullscreen) {
			windowType = type;
			MakeWindow();
		}
		int wid;
		int hei;
		glfwGetWindowSize(glfwWindow, &wid, &hei);
		glViewport(0, 0, wid, hei);
	}
	WindowType Window::GetType() {
		return windowType;
	}
	int Window::GetWidth() {
		int temp;
		glfwGetWindowSize(glfwWindow, &temp, nullptr);
		return temp;
	}
	int Window::GetHeight() {
		int temp;
		glfwGetWindowSize(glfwWindow, nullptr, &temp);
		return temp;
	}
	bool Window::HasFocus() {
		return hasFocus;
	}
	bool Window::IsCursorVisible() {
		return isCursorVisible;
	}
	bool Window::IsCursorLocked() {
		return isCursorLocked;
	}
	void Window::SetCursorVisible(bool visible) {
		isCursorVisible = visible;
		if (visible) glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	void Window::LockCursor(bool locked) {
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
	bool Window::IsOpen() {
		return !glfwWindowShouldClose(glfwWindow);
	}
}