#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Buffer.h"

#include "../Handlers/AssetHandler.h"

#include "../GameState.h"

#include "../Objects/Camera.h"

namespace engone {

	struct Cube {
		glm::mat4 matrix; // includes scale
		glm::vec3 color;
	};
	struct Sphere {
		glm::vec3 position;
		float radius;
		glm::vec3 color;
	};

	// Create buffers/meshes for drawing rectangles. Gui shader should be added before. 
	void InitRenderer(EngoneHint hints);
	void UninitRenderer();
	void EnableBlend();
	void EnableDepth();

	Camera* GetCamera();
	glm::mat4& GetLightProj();
	void SetProjection(float ratio);
	void UpdateViewMatrix(double lag);
	void UpdateProjection(Shader* shader);

	void RenderRenderer();

	/*
	set uniforms before. uPos, USize, uColor, uTextured. Make sure Blending is on, make sure uWindows is set.
	*/
	void DrawString(Font* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar);
	// set guiShader uniforms
	//void DrawRect();
	// set guiShader uniforms
	//void DrawUVRect(float u, float v, float uw, float vh);	

	void DrawCube(glm::mat4 matrix, glm::vec3 scale = {1,1,1}, glm::vec3 color = { 1,1,1 });
	void DrawNetCube(glm::mat4 matrix, glm::vec3 scale = {1,1,1}, glm::vec3 color = { 1,1,1 });
	void DrawSphere(glm::vec3 position, float radius=1, glm::vec3 color = {1,1,1});

	//// Bind shader, set uTransform/uColor, see shader uniforms for more info
	//void DrawCube(float w,float h,float d);
	//// Bind shader, set uTransform/uColor, see shader uniforms for more info
	//void DrawSphere(float radius);

	// Customized mesh
	void DrawBegin();
	void AddVertex(float x,float y,float z);
	void AddIndex(unsigned int a,unsigned int b);
	void DrawBuffer();

	void ClearLines();
	void AddLine(glm::vec3 a, glm::vec3 b);
	void DrawLines();

	namespace ui {
		struct Color {
			float r = 1.f, g = 1.f, b = 1.f, a = 1.f;
		};
		struct Box {
			float x = 0, y = 0, w = 0, h = 0;
			Color rgba;
		};
		struct TexturedBox {
			Texture* texture;
			float x = 0, y = 0, w = 0, h = 0;
			float u = 0, v = 0, uw = 1.f, vh = 1.f;
			Color rgba;
		};
		struct TextBox {
			std::string text;
			float x = 0, y = 0, h = 20;
			Font* font = nullptr;
			Color rgba;
			int at = -1;
			bool edited = false;
		};

		// add a rectangle to the pipeline
		void Draw(Box box);
		void Draw(TexturedBox box);
		void Draw(TextBox& box);

		void Edit(std::string& str, int& at);
		void Edit(std::string& str);
		void Edit(TextBox& box);

		bool Clicked(Box box);
		bool Clicked(Box box);
		bool Hover(Box box);
		bool Hover(TextBox box);
	};
	//void InitUIPipeline();
	void RenderUIPipeline();
}