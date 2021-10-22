#pragma once

#include "../Rendering/Renderer.h"
#include "../Keyboard.h"
#include "../Rendering/Font.h"

namespace engone {
	/*
	Text should fit in a box

	*/
	class Text {
	public:
		Text() {}
		bool center = false;
		bool staticHeight = false;
		void Center(bool f);
		void Setup(const std::string& font, bool cent);

		std::string font;
		void SetFont(const std::string& font);

		std::string text;
		/*
		Updates container
		*/
		//void SetText(std::string text,float h);
		void SetText(const std::string& text);

		float* elemW = nullptr, * elemH = nullptr;
		/*
		Alternative to this function? How does it even work?
		*/
		void ElemWH(float*, float*);

		float xpos = 0, ypos = 0;
		void SetPos(float x, float y);
		/*
		In pixels or not
		*/
		float PixelWidth(bool f);
		float PixelHeight(bool f);
		/*
		Argument can't be more than size of text
		*/
		float ScreenPosX(int c);
		/*
		Argument can't be more than size of text
		*/
		float ScreenPosY(int c);
		/*
		Argument can't be more than size of text
		*/
		int PixelPosX(int c);
		bool elemShiftL = false, elemShiftR = false, elemAltR = false;
		void EditText(int key, int action);

		int CharOnLine(int c);
		bool isPixel = false;
		float charHeight = 0.1;
		void SetHeight(int s);
		void SetHeight(float s);

		float r = 1, g = 1, b = 1, a = 1;
		void SetCol(float r, float g, float b, float a);
		/*
		Update SetPos(x,y)
		*/
		void DrawString(float alpha, bool marker);
		//TriangleBuffer container;
		/*
		Can be altered without complications
		*/
		int atChar = 0;
		//TriangleBuffer marker;
	};
}