#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Buffer.h"

#include "Handlers/AssetHandler.h"

namespace engone {

	// Create buffers/meshes for drawing rectangles. Gui shader should be added before. 
	void InitRenderer();
	void EnableBlend();
	void EnableDepth();

	/*
	set uniforms before. uPos, USize, uColor, uTextured. Make sure Blending is on, make sure uWindows is set.
	*/
	void DrawString(Font* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar);
	// set guiShader uniforms
	void DrawRect();
	// set guiShader uniforms
	void DrawUVRect(float u, float v, float uw, float vh);	

	void UpdateViewMatrix(double lag);
}