#pragma once

#include "../Handlers/AssetHandler.h"

namespace engone {
	/*class Context {
	public:

		std::vector<Box> boxes;
		std::vector<PText> texts;

		struct BoxRef {
			BoxRef(int index, std::vector<Box>* vector) : index(index), vector(vector) {}
			int index;
			std::vector<Box>* vector;
			Box* get() {
				return vector->data()+index;
			}
		};

		void Rect(Box box) {
			boxes.push_back(box);
			//return { boxes.size() - 1,&boxes };
		}
		void String(PText text) {
			texts.push_back(text);
		}
	};*/
	namespace ui {
		struct Box {
			float x = 0, y = 0, w = 0, h = 0;
			float r = 1.f, g = 1.f, b = 1.f, a = 1.f;
		};
		struct TexturedBox {
			Texture* texture;
			float x = 0, y = 0, w = 0, h = 0;
			float u = 0, v = 0, uw = 1.f, vh = 1.f;
			float r = 1.f, g = 1.f, b = 1.f, a = 1.f;
		};
		struct TextBox {
			std::string text;
			float x=0, y=0, h=20;
			Font* font=nullptr;
			float r = 1.f, g = 1.f, b = 1.f, a = 1.f;
			int at=-1;
			bool edited = false;
		};
		struct Circle {
			float x, y, r;
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
	void InitUIPipeline();
	void RenderUIPipeline();
}