#pragma once

#include <GLFW/glfw3.h>

namespace engone {

	enum class WindowMode : char {
		None,
		Windowed,
		Fullscreen,
		BorderlessFullscreen
	};
	class Window {
	private:
		GLFWwindow* m_window;
		WindowMode m_windowMode = WindowMode::None;
		bool m_cursorVisible = true, m_cursorLocked = false, m_focus = true;
		float m_width, m_height;
		std::string m_title = "Untitled";

		float x=-1, y=-1, w=-1, h=-1;

	public:
		friend void FocusCallback(GLFWwindow* window, int focused);
		friend void ResizeCallback(GLFWwindow* window, int width, int height);

		Window(WindowMode mode = WindowMode::Windowed);
		Window(int width, int height, WindowMode mode = WindowMode::Windowed);
		Window(int x, int y, int width, int height, WindowMode mode = WindowMode::Windowed);
		~Window();

		GLFWwindow* glfw() {
			return m_window;
		}

		void setTitle(const std::string title);
		void setActiveContext();

		void setMode(WindowMode mode);
		WindowMode getMode();
		// Width of game screen not window!
		float getWidth();
		// Height of game screen not window!
		float getHeight();
		bool hasFocus();

		/*
		True if the cursor is visible.
		*/
		bool isCursorVisible();
		/*
		If true, the cursor will be made visible.
		*/
		void setCursorVisible(bool visible);
		/*
		Whether the cursor is locked to the window.
		*/
		bool isCursorLocked();
		/*
		If true, the cursor will be made invisible and locked to the window. Use this when you want the player to lock around.
		*/
		void lockCursor(bool locked);

		bool isRunning();
	};

	void InitGLFW();
	Window* GetMappedWindow(GLFWwindow* window);

	Window* GetWindow();
	// Width of game screen not window!
	float GetWidth();
	// Height of game screen not window!
	float GetHeight();
}