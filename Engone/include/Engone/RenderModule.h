#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Engone/Rendering/Buffer.h"
#include "Engone/Rendering/Camera.h"
#include "Engone/AssetModule.h"
#include "Engone/LoopInfo.h"

namespace engone {
	
	class Renderer;
	namespace ui {
		struct Color {
			float r = 1.f, g = 1.f, b = 1.f, a = 1.f;
		};
		struct Box {
			//public:
				//Box() = default;
				//Box(float x, float y, float w, float h, Color rgba) : x(x), y(y), w(w), h(h), rgba(rgba) { }
			float x = 0, y = 0, w = 0, h = 0;
			Color rgba;
		};
		struct TexturedBox {
			//TexturedBox() = default;
			//TexturedBox(Texture* texture, float x, float y, float w, float h, Color rgba) : Box(x,y,w,h,rgba), texture(texture) {}
			Texture* texture;
			float x = 0, y = 0, w = 0, h = 0;
			float u = 0, v = 0, uw = 1.f, vh = 1.f;
			Color rgba;
		};
		// edited needs to be true for the at cursor to be shown
		struct TextBox {
			//TextBox(const std::string& text, float x, float y, float h) : text() {}
			std::string text;
			float x = 0, y = 0, h = 20;
			Font* font = nullptr;
			Color rgba;
			int at = -1;
			bool editing = false;
		};

		void Draw(Box box);
		void Draw(TexturedBox box);
		void Draw(TextBox& box);

		void Edit(std::string& str);
		void Edit(TextBox* text, bool stopEditWithEnter = true);

		bool Clicked(Box& box);
		bool Clicked(TextBox& box);
		bool Hover(Box& box);
		bool Hover(TextBox& box);

		//void SetActiveRenderer(Renderer* renderer);
	}
	void EnableBlend();
	void EnableDepth();
	Renderer* GetActiveRenderer();
	class Window;
	class Renderer {
	public:
		Renderer() {}

		// buffers will be attached to the active window
		void init();

		Camera* getCamera();
		glm::mat4& getLightProj();
		void setProjection(float ratio);
		void updateViewMatrix(double lag);
		void updateProjection(Shader* shader);

		/*
		set uniforms before. uPos, USize, uColor, uTextured. Make sure Blending is on, make sure uWindows is set.
		*/
		void DrawString(Font* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar);

		void DrawCube(glm::mat4 matrix, glm::vec3 scale = { 1,1,1 }, glm::vec3 color = { 1,1,1 });
		void DrawNetCube(glm::mat4 matrix, glm::vec3 scale = { 1,1,1 }, glm::vec3 color = { 1,1,1 });
		void DrawSphere(glm::vec3 position, float radius = 1, glm::vec3 color = { 1,1,1 });

		void DrawLine(glm::vec3 a, glm::vec3 b);
		void DrawTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);
		
		void render(RenderInfo& info);

		Window* getParent() const { return m_parent; }

		// ui::Draw amongst others will use the active renderer.
		void setActiveRenderer();

		static const int TEXT_BATCH = 40;
		static const int MAX_BOX_BATCH = 100;
		static const int VERTEX_SIZE = 2 + 2 + 4 + 1;
	private:

		bool m_initialized = false;

		glm::mat4 lightProjection{};
		glm::mat4 projMatrix{};
		glm::mat4 viewMatrix{};
		float fov = 90.f, zNear = 0.1f, zFar = 400.f;
		
		Window* m_parent=nullptr;

		VertexBuffer pipe3lineVB{};
		IndexBuffer pipe3lineIB{};
		VertexArray pipe3lineVA{};
		std::vector<float> pipe3lines;

		struct Cube {
			glm::mat4 matrix; // includes scale
			glm::vec3 color;
		};
		struct Sphere {
			glm::vec3 position;
			float radius;
			glm::vec3 color;
		};
		VertexArray cubeVAO{};
		VertexBuffer cubeVBO{};
		VertexBuffer cubeInstanceVBO{};
		IndexBuffer cubeIBO{};

		std::vector<Cube> cubeObjects;

		float verts[16 * TEXT_BATCH]{}; // 16 floats, 4 corners for a letter, each corner has x,y,u,v
		VertexBuffer textVBO{};
		VertexArray textVAO{};
		IndexBuffer textIBO{};

		VertexArray boxVAO{};
		VertexBuffer boxVBO{};
		IndexBuffer boxIBO{};
		float floatArray[4 * VERTEX_SIZE * MAX_BOX_BATCH]{};

		ItemVector uiObjects{};
		std::vector<std::string> uiStrings;

		Camera engine_camera{};

		friend void ui::Draw(ui::Box box);
		friend void ui::Draw(ui::TexturedBox box);
		friend void ui::Draw(ui::TextBox& box);
		friend class Window;
		friend class Engone;
	};
}