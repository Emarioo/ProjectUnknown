#pragma once

// #define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//#include "Engone/Rendering/Buffer.h"
//#include "Engone/Rendering/Camera.h"
//#include "Engone/AssetModule.h"
//#include "Engone/LoopInfo.h"
//#include "Engone/Utilities/Utilities.h"

// Base pipeline
#include "Engone/Rendering/RenderPipeline.h"

// Render utility
#include "Engone/Rendering/UIRenderer.h"
#include "Engone/Rendering/CommonRenderer.h"

#ifdef gone
namespace engone {

	void EnableBlend();
	void EnableDepth();
	class Window;
	class Renderer {
	public:
		Renderer() {}

		// buffers will be attached to the active window
		void init();

		Camera* getCamera();
		glm::mat4& getLightProj();
		void setProjection();
		glm::mat4& getProjection();
		void setOrthogonal();
		//void updateViewMatrix(double lag);
		void updateProjection(Shader* shader);
		void updateProjection(Shader* shader, glm::mat4 viewMatrix);

		/*
		set uniforms before. uPos, USize, uColor, uTextured. Make sure Blending is on, make sure uWindows is set.
		Slower but more sophisticated. Function can center and wrap text in a box.
		*/
		void DrawString(FontAsset* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar);
		/*
		set uniforms before. uPos, USize, uColor, uTextured. Make sure Blending is on, make sure uWindows is set.
		Quick version of drawing text
		*/
		void DrawString(FontAsset* font, const std::string& text, float height, int cursorPosition);

		void DrawCube(glm::mat4 matrix, glm::vec3 scale = { 1,1,1 }, glm::vec3 color = { 1,1,1 });
		void DrawNetCube(glm::mat4 matrix, glm::vec3 scale = { 1,1,1 }, glm::vec3 color = { 1,1,1 });
		void DrawSphere(glm::vec3 position, float radius = 1, glm::vec3 color = { 1,1,1 });

		void DrawLine(glm::vec3 a, glm::vec3 b, glm::vec3 rgb = {1.f,0.f,0.f});
		void DrawCubeRaw();

		//void DrawLine(glm::vec3 a, glm::vec3 b);
		void DrawTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);

		//// Uses standard viewMatrix from camera
		//void DrawModel(ModelAsset* modelAsset, glm::mat4 matrix);
		//
		//// Draws model orthogonally
		//void DrawOrthoModel(ModelAsset* modelAsset, glm::mat4 matrix);
		
		void render(LoopInfo& info);

		// will draw a vertex buffer with two triangles.
		// Set shader, texture, uniforms before.
		void DrawQuad(LoopInfo& info);

		Window* getParent() const { return m_parent; }

		// ui::Draw amongst others will use the active renderer.
		void setActiveRenderer();

		VertexBuffer& getInstanceBuffer() { return instanceBuffer; }

		static const int TEXT_BATCH = 40;
		static const int MAX_BOX_BATCH = 100;
		static const int VERTEX_SIZE = 2 + 2 + 4 + 1;
		static const uint32_t INSTANCE_BATCH = 1000;
	private:

		bool m_initialized = false;

		glm::mat4 lightProjection{};
		glm::mat4 projMatrix{};
		//glm::mat4 viewMatrix{};
		float fov = 90.f, zNear = 0.1f, zFar = 400.f;
		// near and far are constants in some particle shaders. don't forget to change them;
		
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

		VertexArray cube2VAO{};
		VertexBuffer cube2VBO{};
		IndexBuffer cube2IBO{}; // NOTE: this is the same as cubeIBO. Could be removed.(altough, not removing this would ensure that changing the other cubes values won't affect this one)

		VertexBuffer quadVB{};
		IndexBuffer quadIB{};
		VertexArray quadVA{};

		VertexBuffer instanceBuffer;

		//struct ModelDraw {
		//	ModelAsset* modelAsset;
		//	glm::mat4 matrix;
		//	bool isOrthogonal;
		//};
		//std::vector<ModelDraw> modelObjects;

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

		Camera camera;

		friend void ui::Draw(ui::Box box);
		friend void ui::Draw(ui::TexturedBox box);
		friend void ui::Draw(ui::TextBox& box);
		friend void ui::Draw(ModelAsset* asset, glm::mat4 matrix);
		friend class Window;
		friend class Engone;
	};
}
#endif