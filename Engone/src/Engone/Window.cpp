
#include "Rendering/Renderer.h"

#include "EventHandler.h"

#include "Window.h"

namespace engone {

	static Window* activeWindow=nullptr;
	Window* GetWindow() {
		return activeWindow;
	}
	float GetWidth() {
		return activeWindow->getWidth();
	}
	float GetHeight() {
		return activeWindow->getHeight();
	}

	static bool glfwIsActive = false;
	static double glfwFreezeTime = 0;
	void InitGLFW() {
		// There is this bug with glfw where it freezes. This will tell you if it has.
		std::thread tempThread = std::thread([] {
			for (int i = 0; i < 20; i++) {
				std::this_thread::sleep_for((std::chrono::milliseconds)100);
				if (glfwIsActive) {
					return;
				}
			}
			std::cerr << "GLFW has frozen... ";
			glfwFreezeTime = GetSystemTime();
			});
		if (!glfwInit()) {
			std::cout << "Glfw Init error!" << std::endl;
			return;
		}
		if (glfwFreezeTime != 0)
			std::cout << "it took " << (GetSystemTime() - glfwFreezeTime) << " seconds\n";
		
		glfwIsActive = true;
		tempThread.join();
	}
	static std::unordered_map<GLFWwindow*, Window*> windowMapping;
	Window* GetMappedWindow(GLFWwindow* window) {
		if (windowMapping.count(window)) {
			return windowMapping[window];
		}
		return nullptr;
	}
	static void FocusCallback(GLFWwindow* window, int focused) {
		auto win = windowMapping.find(window);
		if (win != windowMapping.end()) {
			win->second->m_focus = focused;
			if (focused)
				activeWindow = win->second;
		}
	}
	static void ResizeCallback(GLFWwindow* window, int width, int height) {
		auto win = windowMapping.find(window);
		if (win != windowMapping.end()) {
			win->second->m_width = width;
			win->second->m_height = height;
		}
	}
	static void CloseCallback(GLFWwindow* window) {
		auto win = windowMapping.find(window);
		if (win != windowMapping.end()) {
			if(win->second->closeCallback)
				win->second->closeCallback();
		}
	}
	Window::Window(WindowMode mode) {
		// setup
		if (!glfwIsActive) {
			InitGLFW();
		}

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
		
		x = vidmode->width / 6;
		y = vidmode->height / 6;
		w = vidmode->width / 1.5;
		h = vidmode->height / 1.5;

		setMode(mode);

		GLenum err = glewInit();
		if (err != GLEW_OK) {
			std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
			return;
		}
	}
	Window::Window(int width, int height, WindowMode mode) {
		// setup
		if (!glfwIsActive) {
			InitGLFW();
		}

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

		w = width;
		h = height;
		x = vidmode->width / 2-w/2;
		y = vidmode->height / 2-h/2;

		setMode(mode);

		GLenum err = glewInit();
		if (err != GLEW_OK) {
			std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
			return;
		}
	}
	Window::Window(int x, int y, int width, int height, WindowMode mode) {
		// setup
		if (!glfwIsActive) {
			InitGLFW();
		}

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);

		this->x = x;
		this->y = y;
		w = width;
		h = height;

		setMode(mode);

		GLenum err = glewInit();
		if (err != GLEW_OK) {
			std::cout << "GLEW Error: " << glewGetErrorString(err) << std::endl;
			return;
		}
	}
	Window::~Window() {
		if (activeWindow == this) {
			if (windowMapping.size() > 0) {
				activeWindow = windowMapping.begin()->second;
			} else {
				log::out << "Bad crash " << __FILE__ << " " << (int)__LINE__ << "\n";
			}
		}
		windowMapping.erase(m_window);
		glfwDestroyWindow(m_window);
	}
	void Window::setTitle(const std::string title) {
		m_title = title;
		glfwSetWindowTitle(m_window, title.c_str());
	}
	void Window::setActiveContext() {
		glfwMakeContextCurrent(m_window);
		int wid;
		int hei;
		glfwGetWindowSize(m_window, &wid, &hei);
		m_width = wid;
		m_height = hei;
		glViewport(0, 0, wid, hei);
		activeWindow = this;
	}
	void Window::setMode(WindowMode winMode) {
		//if (m_windowMode == winMode) return;
		// 
		// last monitor this window or screen was in
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		if (!m_window) {
			//x = mode->width / 6;
			//y = mode->height / 6;
			//w = mode->width / 1.5;
			//h = mode->height / 1.5;
			//glfwWindowHint(GLFW_FOCUS_ON_SHOW, true);
			if (winMode == WindowMode::Windowed) {
				glfwWindowHint(GLFW_DECORATED, true);
				m_window = glfwCreateWindow(w, h, m_title.c_str(), NULL, NULL);
				glfwSetWindowPos(m_window,x,y);
				//glfwSetWindowMonitor(m_window, NULL, x, y, w, h, mode->refreshRate);
			} else if (winMode == WindowMode::Fullscreen) {
				glfwWindowHint(GLFW_DECORATED, true);
				m_window = glfwCreateWindow(mode->width, mode->height, m_title.c_str(), monitor, NULL);
			} else if (winMode == WindowMode::BorderlessFullscreen) {
				glfwWindowHint(GLFW_DECORATED, false);
				//glfwWindowHint(GLFW_FLOATING, true);
				m_window = glfwCreateWindow(mode->width, mode->height, m_title.c_str(), NULL, NULL);
				glfwSetWindowPos(m_window, 0, 0);
				//glfwSetWindowMonitor(m_window, NULL, 0, 0, mode->width, mode->height, mode->refreshRate);
			}
			windowMapping[m_window] = this;

			glfwSetWindowFocusCallback(m_window, FocusCallback);
			glfwSetWindowSizeCallback(m_window, ResizeCallback);
			glfwSetWindowCloseCallback(m_window, CloseCallback);
			InitEvents(m_window);
		} else {
			if (winMode == WindowMode::Windowed) {
				glfwWindowHint(GLFW_DECORATED, true);
				glfwWindowHint(GLFW_FLOATING, false);
				glfwSetWindowMonitor(m_window, NULL, x, y, w, h, mode->refreshRate);
			} else if (winMode == WindowMode::Fullscreen) {
				glfwWindowHint(GLFW_DECORATED, true);
				glfwWindowHint(GLFW_FLOATING, false);
				glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			} else if (winMode == WindowMode::BorderlessFullscreen) {
				glfwWindowHint(GLFW_DECORATED, false);
				glfwWindowHint(GLFW_FLOATING, true);
				glfwSetWindowMonitor(m_window, NULL, 0, 0, mode->width, mode->height, mode->refreshRate);
			}
		}
		m_focus = true;

		m_windowMode = winMode;

		setActiveContext();	
	}
	WindowMode Window::getMode() {
		return m_windowMode;
	}
	float Window::getWidth() {
		return m_width;
	}
	float Window::getHeight() {
		return m_height;
	}
	bool Window::hasFocus() {
		return m_focus;
	}
	bool Window::isCursorVisible() {
		return m_cursorVisible;
	}
	void Window::setCursorVisible(bool visible) {
		m_cursorVisible = visible;
		if (visible) glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	bool Window::isCursorLocked() {
		return m_cursorLocked;
	}
	void Window::lockCursor(bool locked) {
		m_cursorLocked = locked;
		if (locked) {
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		} else {
			setCursorVisible(isCursorVisible());
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}
	}
	bool Window::isRunning() {
		return !glfwWindowShouldClose(m_window);
	}

	static std::string title = "Unnamed";

	// relevant if windowed, stores the coordinates when going from windowed->fullscreen->windowed
	static int x=0, y=0, w=0, h=0;

	static bool isCursorVisible = true, isCursorLocked = false, hasFocus = true;
	/*
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
	void SetActiveContext() {
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
	}*/
}