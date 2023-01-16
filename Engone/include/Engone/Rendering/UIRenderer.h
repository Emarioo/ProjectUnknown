#pragma once

#include "Engone/AssetModule.h"
#include "Engone/Rendering/RenderPipeline.h"
#include "Engone/Utilities/Utilities.h"

// It is here to avoid including glfw3.h
#ifndef GLFW_MOUSE_BUTTON_1
#define GLFW_MOUSE_BUTTON_1 0
#endif

#define GET_UI_RENDERER() (CommonRenderer*)engone::GetActiveWindow()->getPipeline()->getComponent("ui_renderer");

namespace engone {

	//class UIRenderer : public RenderComponent {
	//public:
	//	UIRenderer() = default;

	//	void render(LoopInfo& info) override;

	//	void setActiveUIRenderer();

	//	friend void ui::Draw(ui::Box box);
	//	friend void ui::Draw(ui::TexturedBox box);
	//	friend void ui::Draw(ui::TextBox& box);
	//	friend void ui::Draw(ModelAsset* asset, glm::mat4 matrix);

	//private:

	//	ItemVector uiObjects{};
	//	std::vector<std::string> uiStrings;
	//};
	//UIRenderer* GetActiveUIRenderer();
	namespace ui {
		// r,g,b,a OR rgba OR rgb,a
		struct Color {
			Color() = default;
			Color(float rgba) : r(rgba), g(rgba), b(rgba), a(rgba) {}
			Color(float rgb, float a) : r(rgb), g(rgb), b(rgb), a(a) {}
			Color(float r, float g, float b) : r(r), g(g), b(b), a(1) {}
			Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
			float r = 1.f, g = 1.f, b = 1.f, a = 1.f;

			Color(glm::vec4 vec) { *this = *(ui::Color*)&vec; }

			glm::vec4 toVec4() {
				return *(glm::vec4*)this;
			}
		};
		// x,y,w,h,color
		struct Box {
			//public:
				//Box() = default;
				//Box(float x, float y, float w, float h, Color rgba) : x(x), y(y), w(w), h(h), rgba(rgba) { }
			float x = 0, y = 0, w = 0, h = 0;
			Color rgba;
		};
		// texture,x,y,w,h,u,v,uw,vh,color
		struct TexturedBox {
			//TexturedBox() = default;
			//TexturedBox(Texture* texture, float x, float y, float w, float h, Color rgba) : Box(x,y,w,h,rgba), texture(texture) {}
			Texture* texture;
			float x = 0, y = 0, w = 0, h = 0;
			float u = 0, v = 0, uw = 1.f, vh = 1.f;
			Color rgba;
		};
		// text,x,y,h,font,color
		// edited needs to be true for the at cursor to be shown
		struct TextBox {
			//TextBox(const std::string& text, float x, float y, float h) : text() {}
			std::string text;
			float x = 0, y = 0, h = 20;
			FontAsset* font = nullptr;
			Color rgba;
			int at = -1;
			bool editing = false;

			float getWidth();
			// \n is taken into account
			float getHeight();
		};

		void Draw(Box box);
		void Draw(TexturedBox box);
		void Draw(TextBox& box);

		void Draw(ModelAsset* modelAsset, glm::mat4 matrix);

		void Edit(std::string& str, int& at, bool& editing, bool stopEditWithEnter = true);
		void Edit(std::string& str);
		void Edit(TextBox* text, bool stopEditWithEnter = true);

		// 1: clicked, 0: didn't click, -1: clicked but missed box
		int Clicked(Box& box, int mouseKey = GLFW_MOUSE_BUTTON_1);
		// 1: clicked, 0: didn't click, -1: clicked but missed box
		int Clicked(TextBox& box, int mouseKey = GLFW_MOUSE_BUTTON_1);
		bool Hover(Box& box);
		bool Hover(TextBox& box);
	}
	class UIRenderer : public RenderComponent {
	public:
		UIRenderer() : RenderComponent("ui_renderer", 99) {}
		~UIRenderer();

		void render(LoopInfo& info) override;

		void setActiveUIRenderer();

		VertexBuffer& getInstanceBuffer();

		static const int MAX_BOX_BATCH = 100;
		static const int VERTEX_SIZE = 2 + 2 + 4 + 1;
		static const uint32_t INSTANCE_BATCH = 1000;
	private:
		ItemVector uiObjects{};
		std::vector<std::string> uiStrings;

		VertexBuffer instanceBuffer;

		VertexArray boxVAO{};
		VertexBuffer boxVBO{};
		IndexBuffer boxIBO{};
		float floatArray[4 * VERTEX_SIZE * MAX_BOX_BATCH]{};

		
		friend void ui::Draw(ui::Box box);
		friend void ui::Draw(ui::TexturedBox box);
		friend void ui::Draw(ui::TextBox& box);
		friend void ui::Draw(ModelAsset* asset, glm::mat4 matrix);
	};
	UIRenderer* GetActiveUIRenderer();
}