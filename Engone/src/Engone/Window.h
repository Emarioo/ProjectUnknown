#pragma once

#include "Module.h"

#include "GLFW/glfw3.h"

namespace engone {
	enum class WindowType {
		None,
		Windowed,
		Fullscreen,
		BorderlessFullscreen
	};
	class Window : public Module {
	public:
		Window();
		Window(const std::string& title="Unnamed");
		~Window() override;
		
		void Init() override;
		void Update(float delta) override;
		void Render() override;

		void SetTitle(const std::string& title);

		// Create or recreate window with new settings
		void MakeWindow();
		
		void SetType(WindowType type);
		WindowType GetType();

		// Width of game screen not window!
		int GetWidth();
		// Height of game screen not window!
		int GetHeight();

		bool HasFocus();

		/*
		@return: true if the cursor is visible.
		*/
		bool IsCursorVisible();
		/*
		@visible: if true, the cursor will be made visible.
		*/
		void SetCursorVisible(bool visible);
		/*
		@return: whether the cursor is locked to the window.
		*/
		bool IsCursorLocked();
		/*
		@locked: if true, the cursor will be made invisible and locked to the window. Use this when you want the player to lock around.
		*/
		void LockCursor(bool locked);

		bool IsOpen();

	private:
		GLFWwindow* glfwWindow;
		WindowType windowType;

		std::string title = "Unnamed";

		// relevant if windowed
		int x, y, w, h;

		bool isCursorVisible = true, isCursorLocked = false, hasFocus=false;

	};
}