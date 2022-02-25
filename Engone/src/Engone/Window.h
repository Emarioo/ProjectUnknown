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
	GLFWwindow* GetWindow();

	void InitWindow(const std::string& title = "Unnamed");

	void SetTitle(const std::string& title);

	// Create or recreate window with new settings, when none, last or default will be used.
	void MakeWindow(WindowType type = WindowType::None);

	void SetActiveContext();

	// Set the type of the window. Will recreate window if necessary
	void SetType(WindowType type);
	WindowType GetType();

	// Width of game screen not window!
	float GetWidth();
	// Height of game screen not window!
	float GetHeight();

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
}