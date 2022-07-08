#pragma once

#include "Rendering/Renderer.h"

namespace engone {

	/*
	Init usage of windows console.
	*/
	void InitEvents();

	// Called absolute first of update loop, if using windows console events
	void RefreshEvents();
	// Called absolute last of update loop, always necessary
	void ResetEvents();

	int GetMouseX();
	int GetMouseY();
	/*
	return 0 if no scroll and 1 or -1 if scrolled.
	*/
	int IsScrolledY(bool isGlfw = true);
	/*
	return 0 if no scroll and 1 or -1 if scrolled.
	*/
	int IsScrolledX(bool isGlfw = true);

	/*
	virtualKeyCode: Window or glfw virtual key code depending on what you are using.
	return true if the key or mouse button is down.
	*/
	bool IsKeyDown(int virtualKeyCode, bool isGlfw = true);
	/*
	virtualKeyCode: Windows.h or glfw virtual key code depending on what you are using.
	return true if the key was pressed this refresh/frame/update.
	*/
	bool IsKeyPressed(int virtualKeyCode, bool isGlfw = true);
}