#include "gonpch.h"

#include "UIPipeline.h"

#include "../Rendering/Buffer.h"

#include "../Handlers/AssetHandler.h"

#include "../EventHandler.h"

#include "../Window.h"

static const char* uiPipelineGLSL = {
#include "../Shaders/uiPipeline.glsl"
};

namespace engone {

	static const int MAX_RECT_BATCH = 100;
	static const int VERTEX_SIZE = 2+2+4+1;
	static VertexArray vertexArray;
	static VertexBuffer boxBuffer;
	static IndexBuffer indexBuffer;
	static float floatArray[4 * VERTEX_SIZE * MAX_RECT_BATCH];

	static std::vector<ui::Box> boxes;
	static std::unordered_map<Texture*,std::vector<ui::TexturedBox>> texturedBoxes;
	static std::vector<ui::TextBox> texts;

	// global ui pipeline
	namespace ui {

		// There is a limit to how many rects you can have thanks to float[], having a std::vector and batching rendering would be better
		void Draw(Box box) {
			boxes.push_back(box);
		}
		void Draw(TexturedBox box) {
			if (box.texture == nullptr) {
				return;
			}
			if (texturedBoxes.count(box.texture) == 0) {
				texturedBoxes[box.texture] = std::vector<TexturedBox>();
			}
			texturedBoxes[box.texture].push_back(box);
		}
		void Draw(TextBox& box) {
			if (!box.edited) {
				box.at = -1;
			} else
				box.edited = false;
			texts.push_back(box);
		}

		void Edit(std::string& str, int& at) {
			uint32_t chr;
			while (chr = PollChar()) {
				if (chr == GLFW_KEY_BACKSPACE) {
					if (str.length() > 0 && at > 0) {
						// 012345

						str = str.substr(0, at - 1) + str.substr(at);
						at--;
					}
				} else if (chr == GLFW_KEY_ENTER) {
					str.insert(str.begin() + at, '\n');
					at++;
				} else if (chr == GLFW_KEY_DELETE) {
					if (str.length() > at) {
						str = str.substr(0, at) + str.substr(at + 1);
					}
				} else if (chr == GLFW_KEY_LEFT) {
					if (at > 0)
						at--;
				} else if (chr == GLFW_KEY_RIGHT) {
					if (at < str.length())
						at++;
				} else {
					str.insert(str.begin() + at, chr);
					at++;
				}
			}
		}
		void Edit(std::string& str) {
			int at = str.length();
			Edit(str, at);
		}
		void Edit(TextBox& box) {
			if(box.at==-1)
				box.at = box.text.length();
			box.edited = true;
			Edit(box.text, box.at);
		}

		static bool inside(Box box, float x, float y) {
			return box.x<x&& box.x + box.w>x && box.y<y&& box.y + box.h>y;
		}
		bool Clicked(Box box) {
			if (IsKeyPressed(GLFW_MOUSE_BUTTON_1)) {
				float x = GetMouseX();
				float y = GetMouseY();
				if (inside(box, x, y)) {
					return true;
				}
			}
			return false;
		}
		bool Hover(Box box) {
			float x = GetMouseX();
			float y = GetMouseY();
			if (inside(box, x, y)) {
				return true;
			}
			return false;
		}
		bool Hover(TextBox box) {
			float x = GetMouseX();
			float y = GetMouseY();
			if (box.font == nullptr) {
				if (inside({ box.x,box.y,box.h * box.text.length(),box.h }, x, y)) {
					return true;
				}
			} else {

				if (inside({ box.x, box.y, box.font->GetWidth(box.text,box.h), box.h }, x, y)) {
					return true;
				}
			}
			return false;
		}
	}
	void InitUIPipeline() {

		AddAsset<Shader>("uiPipeline", new Shader(uiPipelineGLSL));

		boxBuffer.setData(4 * VERTEX_SIZE * MAX_RECT_BATCH, nullptr);

		uint32_t intArray[6 * MAX_RECT_BATCH];
		for (int i = 0; i < MAX_RECT_BATCH; i++) {
			intArray[i * 6] = i * 4 + 0;
			intArray[i * 6 + 1] = i * 4 + 1;
			intArray[i * 6 + 2] = i * 4 + 2;
			intArray[i * 6 + 3] = i * 4 + 2;
			intArray[i * 6 + 4] = i * 4 + 3;
			intArray[i * 6 + 5] = i * 4 + 0;
		}
		indexBuffer.setData(6 * MAX_RECT_BATCH, intArray);

		vertexArray.addAttribute(2); // pos
		vertexArray.addAttribute(2); // uv
		vertexArray.addAttribute(4); // color
		vertexArray.addAttribute(1, &boxBuffer); // texture
	}
	static struct VERTEX {
		float data[VERTEX_SIZE];
	};
	void RenderUIPipeline() {
		
		//log::out << "pipe\n";
		// setup
		Shader* shad = GetAsset<Shader>("uiPipeline");
		if (!shad) return;

		EnableBlend();
		shad->bind();
		shad->setVec2("uWindow", { GetWidth(), GetHeight() });

		for (int i = 0; i < 8;i++) {
			shad->setInt("uSampler["+std::to_string(i)+std::string("]"), i);
		}
		
		if (boxes.size() > 0) {
			int boxIndex = 0;
			for (int i = 0; i < boxes.size();i++) {
				ui::Box& box = boxes[i];

				((VERTEX*)&floatArray)[boxIndex * 4 + 0] = { box.x, box.y, 0, 0, box.r, box.g, box.b, box.a, -1 };
				((VERTEX*)&floatArray)[boxIndex * 4 + 1] = { box.x, box.y + box.h, 0, 0, box.r, box.g, box.b, box.a, -1 };
				((VERTEX*)&floatArray)[boxIndex * 4 + 2] = { box.x + box.w, box.y + box.h, 0, 0, box.r, box.g, box.b, box.a, -1 };
				((VERTEX*)&floatArray)[boxIndex * 4 + 3] = { box.x + box.w, box.y, 0, 0, box.r, box.g, box.b, box.a, -1 };

				boxIndex++;

				if (boxIndex == MAX_RECT_BATCH || i+1==boxes.size()) {
					if (boxIndex != MAX_RECT_BATCH)
						ZeroMemory(floatArray + boxIndex * 4 * VERTEX_SIZE, (MAX_RECT_BATCH - boxIndex) * 4 * VERTEX_SIZE * sizeof(float));

					boxBuffer.setData(MAX_RECT_BATCH * 4 * VERTEX_SIZE, floatArray);
					vertexArray.draw(&indexBuffer);
				}
			}

			// cleanup
			boxes.clear();
		}
		if (texturedBoxes.size() > 0) {

			int textureSlot=0;
			int boxIndex = 0;
			int totalTextures = 0;
			for (auto [texture, vector] : texturedBoxes) {
				texture->bind(textureSlot);

				for(int i=0;i<vector.size();i++){
					ui::TexturedBox& box = vector[i];

					((VERTEX*)&floatArray)[boxIndex * 4 + 0] = { box.x,			box.y,			box.u,			box.v,				box.r, box.g, box.b, box.a, (float)textureSlot };
					((VERTEX*)&floatArray)[boxIndex * 4 + 1] = { box.x,			box.y + box.h,	box.u,			box.v + box.vh,		box.r, box.g, box.b, box.a, (float)textureSlot };
					((VERTEX*)&floatArray)[boxIndex * 4 + 2] = { box.x + box.w, box.y + box.h,	box.u + box.uw,	box.v + box.vh,		box.r, box.g, box.b, box.a, (float)textureSlot };
					((VERTEX*)&floatArray)[boxIndex * 4 + 3] = { box.x + box.w, box.y,			box.u + box.uw,	box.v,				box.r, box.g, box.b, box.a, (float)textureSlot };
					
					/*for (int i = 0; i < 4;i++) {
						log::out << floatArray[boxIndex * VERTEX_SIZE * 4 + i * VERTEX_SIZE+2] << " " << floatArray[boxIndex * VERTEX_SIZE * 4 + i * VERTEX_SIZE+3] << "\n";
					}*/

					boxIndex++;
					if (boxIndex == MAX_RECT_BATCH) {
						boxIndex = 0;

						boxBuffer.setData(MAX_RECT_BATCH * 4 * VERTEX_SIZE, floatArray);
						vertexArray.draw(&indexBuffer);
					}
				}
				textureSlot++;
				totalTextures++;
				if ((textureSlot == 8 || totalTextures==texturedBoxes.size())&&boxIndex!=0) {
					textureSlot = 0;
					ZeroMemory(floatArray + boxIndex * 4 * VERTEX_SIZE, (MAX_RECT_BATCH - boxIndex) * 4 * VERTEX_SIZE * sizeof(float));
					
					boxBuffer.setData(MAX_RECT_BATCH * 4 * VERTEX_SIZE, floatArray);
					vertexArray.draw(&indexBuffer);
				}
			}

			// cleanup
			texturedBoxes.clear();
		}
		if (texts.size() > 0) {
			Shader* shad = GetAsset<Shader>("gui");
			if (!shad) return;

			EnableBlend();
			shad->bind();
			shad->setVec2("uWindow", { GetWidth(), GetHeight() });

			Font* font = GetAsset<Font>("consolas");
			for (int i = 0; i < texts.size(); i++) {
				ui::TextBox& text = texts[i];

				shad->setVec2("uPos", { text.x, text.y });
				shad->setVec2("uSize", { 1, 1 });
				shad->setVec4("uColor", text.r, text.g, text.b, text.a);
				DrawString(font, text.text, false, text.h, 9999, text.h, text.at);
			}

			texts.clear();
		}
	}
}