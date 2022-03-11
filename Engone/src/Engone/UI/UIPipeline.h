#pragma once

namespace engone {
	struct Box {
		float x=0, y=0, w=0, h=0, r=1.f, g=1.f, b=1.f, a=1.f;
	};
	struct Text {
		float x = 0, y = 0, w = 0, h = 0, r = 1.f, g = 1.f, b = 1.f, a = 1.f;
	};
	struct Circle {
		float x, y, r;
	};
	class Pipeline {
	public:

		std::vector<Box> boxes;
		std::vector<Text> texts;

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
		void Text(Text text) {
			texts.push_back(text);
		}
	};
	namespace ui {
		// add a rectangle to the pipeline
		void Rect(Box box);
		void String(Text box);

		bool Clicked(Box box);
		bool Hover(Box box);
	};
	void InitUIPipeline();
	void RenderUIPipeline();
}