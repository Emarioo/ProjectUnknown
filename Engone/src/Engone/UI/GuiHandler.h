#pragma once

#include "../EventHandler.h"
#include "../Handlers/AssetHandler.h"
#include "../Rendering/Texture.h"

namespace engone
{
	class IElement;
	class ConstraintX
	{
	public:
		ConstraintX(IElement* p);
		IElement* Center(float pos, bool pixelated = true, IElement* stick = nullptr);
		IElement* Left(float pos, bool pixelated = true, IElement* stick = nullptr);
		IElement* Right(float pos, bool pixelated = true, IElement* stick = nullptr);
		float Value();
	private:
		bool pixelSpace;
		float raw = 0;
		IElement* element = nullptr;
		IElement* parent = nullptr;
		int side = 0;
	};
	class ConstraintY
	{
	public:
		ConstraintY(IElement* p);
		IElement* Center(float pos, bool pixelated = true, IElement* stick = nullptr);
		IElement* Bottom(float pos, bool pixelated = true, IElement* stick = nullptr);
		IElement* Top(float pos, bool pixelated = true, IElement* stick = nullptr);
		float Value();
	private:
		bool pixelSpace;
		float raw = 0;
		IElement* parent = nullptr;
		IElement* element = nullptr;
		int side = 0;
	};
	class ConstraintW
	{
	public:
		ConstraintW(IElement* p);
		IElement* Center(float pos, bool pixelated = true, IElement* stick = nullptr);
		float Value();
	private:
		bool pixelSpace;
		float raw = 0;
		IElement* parent = nullptr;
		IElement* element = nullptr;
	};
	class ConstraintH
	{
	public:
		ConstraintH(IElement* p);
		IElement* Center(float pos, bool pixelated = true, IElement* stick = nullptr);
		float Value();
	private:
		bool pixelSpace;
		float raw = 0;
		IElement* element = nullptr;
		IElement* parent = nullptr;
	};
	class Color
	{
	public:
		float r, g, b, a;
		Color();
		Color(float rgb);
		Color(int rgb);
		Color(float rgb, float alpha);
		Color(int rgb, int alpha);
		Color(float red, float green, float blue);
		Color(int red, int green, int blue);
		Color(float red, float green, float blue, float alpha);
		Color(int red, int green, int blue, int alpha);
	};
	class Transition
	{
	private:
		float fadeS = 0, moveS = 0, sizeS = 0;
		float x = 0, y = 0, w = 0, h = 0;
	public:
		bool fade = false, move = false, size = false;
		//float r = 1, g = 1, b = 1, a = 1;
		Color color;
		float fadeT = 0, moveT = 0, sizeT = 0;

		std::string name;
		EventType eventType;

		Transition(const std::string& name);

		bool active = false;
		void Active(bool f);
		Transition* Fade(const Color& c, float time);
		// In pixels
		Transition* Move(float x, float y, float time);
		// In pixels
		Transition* Size(float w, float h, float time);

		float ValueX();
		float ValueY();
		float ValueW();
		float ValueH();

		// Fade is calculated in element.cpp since it requires some more hardcore calculations
		// Might be possible to make a functions out of it but ehh, whatever
		void Update(float delta);
	};

#ifdef gone
	class Text
	{
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
	void Text::Setup(const std::string& _font, bool cent)
	{
		//elemSize = s;
		font = _font;
		center = cent;
		/*
		container.Setup(true, nullptr, 4 * 4 * maxChar, nullptr, 6 * maxChar);
		container.SetAttrib(0, 4, 4, 0);
		unsigned int i[]{
			2,1,0,
			0,3,2
		};

		marker.Setup(true, nullptr, 4 * 4, i, 6);
		marker.SetAttrib(0, 4, 4, 0);
		*/
	}
	void Text::SetPos(float x, float y)
	{
		xpos = x;
		ypos = y;
	}
	void Text::Center(bool f)
	{
		center = f;
	}
	void Text::SetFont(const std::string& _font)
	{
		font = _font;
	}
	void Text::SetHeight(int s)
	{
		charHeight = s;
		isPixel = true;
		staticHeight = true;
	}
	void Text::SetHeight(float s)
	{
		charHeight = s;
		staticHeight = true;
	}
	void Text::SetCol(float r, float g, float b, float a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;

	}
	float Text::PixelWidth(bool f)
	{
		Font* fo = GetFont(font);
		if (fo == nullptr)
			return 0;
		float wid = 0;
		float max = 0;
		for (char cha : text) {
			if (cha == '\n') {
				wid = 0;
				continue;
			}
			int charInd = cha;
			if (charInd < 0)
				charInd += 256;
			wid += fo->charWid[charInd];
			if (wid > max) {
				max = wid;
			}
		}
		if (!f) {
			max *= (charHeight / (16 / 9.f)) / fo->charSize;
		}
		return max;
	}
	float Text::PixelHeight(bool f)
	{
		Font* fo = GetFont(font);
		if (fo == nullptr)
			return 0;
		float hei = fo->charSize;
		for (char cha : text) {
			if (cha == '\n') {
				hei += fo->charSize;
			}
		}
		if (!f) {
			hei *= charHeight / fo->charSize;
		}
		return hei;
	}
	float Text::ScreenPosX(int c)
	{
		Font* fo = GetFont(font);
		if (fo == nullptr)
			return 0;
		float wid = 0;
		for (int i = 0; i < c; i++) {
			int cha = text.at(i);
			if ((char)cha == '\n') {
				wid = 0;
				continue;
			}
			if (cha < 0) {
				cha += 256;
			}
			wid += fo->charWid[cha];
		}
		return wid * ((charHeight / (16 / 9.f)) / fo->charSize);
	}

	int Text::PixelPosX(int c)
	{
		Font* fo = GetFont(font);
		if (fo == nullptr)
			return 0;
		float wid = 0;
		for (int i = 0; i < c; i++) {
			char cha = text.at(i);
			if (cha == '\n') {
				wid = 0;
				continue;
			}
			//std::cout << cha <<" "<< (int)cha << " "<<(int)(cha+256)<<std::endl;
			if (cha < 0)
				cha += 256;
			wid += fo->charWid[cha];
		}
		return wid;
	}
	float Text::ScreenPosY(int c)
	{
		float hei = 0;
		for (int i = 0; i < c; i++) {
			char cha = text.at(i);
			if (cha == '\n') {
				hei++;
			}
		}
		return hei * charHeight;
	}
	void Text::EditText(int key, int action)
	{
		//text.ElemWH(GetW(), GetH()); this is neccessary!
		if (key == GLFW_KEY_LEFT_SHIFT) {
			elemShiftL = action;
		} if (key == GLFW_KEY_RIGHT_SHIFT) {
			elemShiftR = action;
		}
		else if (key == GLFW_KEY_RIGHT_ALT) {
			elemAltR = action;
		}
		else if (key == GLFW_KEY_UP) {
			if (action) {
				int c = atChar;
				int l = CharOnLine(c);
				int newC = -1;
				//std::cout << "start " << c << " " << l << std::endl;

				for (int i = c; i >= 0; i--) {
					if (i >= text.length())
						continue;
					if (c == i && text.at(c) == '\n')
						continue;
					//std::cout << ">" << text.text.at(i) <<" "<<i<< std::endl;
					if (text.at(i) == '\n') {
						int o = 0;
						for (int j = i - 1; j >= 0; j--) {
							//std::cout <<" >" <<text.text.at(j)<<" "<<j << std::endl;
							if (text.at(j) == '\n') {
								o = j + 1;
								break;
							}
							if (j == 0) {
								o = 0;
							}
						}
						if (o + l > i)
							newC = i;
						else
							newC = o + l;
						break;
					}
				}
				if (newC < 0) {
					newC = 0;
				}
				//std::cout << "out " << newC << std::endl;
				atChar = newC;
				/* For uneven width on characters - TODO: Use the closest character instead of the last one
				int c = text.atChar;
				int newC = c;
				float widO = text.PixelPosX(text.atChar);
				float wid = -1;
				for (int i = c; i >= 0; i--) {
					char cha = text.text.at(i);
					if (cha == '\n') {
						if (wid > 0) {
							newC = i;
							break;
						}
						wid = 0;
						if (i != 0) {
							for (int j = i-1; j >= 0; j--) {
								char cha2 = text.text.at(j);
								if (cha2 == '\n') {
									break;
								}
								wid += text.font->charWid[cha2];
							}
						}
						continue;
					}
					if (cha < 0)
						cha += 256;
					if (wid >= 0) {
						wid -= text.font->charWid[cha];
						//std::cout << cha << " " << i << " " << widO << " < " << wid << std::endl;
						if (widO > wid) {
							newC = i;
							if(text.text.length()>i+1){
								if (text.text.at(i + 1)=='\n') {
									newC++;
								}
							}
							break;
						}
					}
				}
				text.atChar = newC;
				*/
			}
		}
		else if (key == GLFW_KEY_LEFT) {
			if (action) {
				if (atChar > 0)
					atChar--;
			}
		}
		else if (key == GLFW_KEY_DOWN) {
			if (action) {
				int c = atChar;
				int l = CharOnLine(c);
				int newC = -1;
				int o = 0;
				//std::cout << "start " << c <<" "<<l<< std::endl;

				for (int i = c; i < text.length(); i++) {
					char cha = text.at(i);
					//std::cout << "ch " << cha << std::endl;
					if (newC >= 0) {
						if (newC == l) {
							newC = l + o + 1;
							//std::cout << "end1 " << newC << std::endl;
							break;
						}
						newC++;
					}
					if (cha == '\n') {
						if (newC >= 0) {
							if (newC < l + 1) {
								newC = i;
								break;
							}
							newC += o + 1;
							//std::cout << "end2 " << newC << std::endl;
							break;
						}
						o = i;
						newC = 0;
						continue;
					}
					if (i == text.length() - 1) {
						if (newC < 0) {
							break;
						}
						newC += o + 1;
					}
				}
				if (newC < 0) {
					newC = text.length();
				}
				//std::cout << "out " << newC << std::endl;
				atChar = newC;

				/* characters width different widths
				int c = text.atChar;
				int newC = c;
				float widO = text.PixelPosX(text.atChar);
				float wid = -1;
				for (int i = c; i < text.text.length(); i++) {
					char cha = text.text.at(i);
					if (cha == '\n') {
						if (wid > 0) {
							newC = i;
							break;
						}
						wid = 0;
						continue;
					}
					if (cha < 0)
						cha += 256;
					if(wid>=0)
						wid += text.font->charWid[cha];
					//std::cout << cha << " " << i << " " << widO << " < " << wid << std::endl;
					if (widO < wid) {
						newC = i;
						break;
					}
				}
				text.atChar = newC;
				*/
			}
		}
		else if (key == GLFW_KEY_RIGHT) {
			if (action) {
				if (atChar < text.size())
					atChar++;
			}
		}
		else if (key == GLFW_KEY_DELETE) {
			if (action) {
				if (text.size() > 0 && atChar < text.size()) {
					if (atChar == 0) {
						SetText(text.substr(1));
					}
					else {
						SetText(text.substr(0, atChar) + text.substr(atChar + 1));
					}
				}
			}
		}
		else if (key == GLFW_KEY_BACKSPACE) {
			if (action) {
				if (text.length() > 0 && atChar > 0) {
					if (atChar == text.size()) {
						SetText(text.substr(0, atChar - 1));
						atChar--;
					}
					else {
						SetText(text.substr(0, atChar - 1) + text.substr(atChar));
						atChar--;
					}
				}
			}
		}
		else if (key == GLFW_KEY_ENTER) {
			if (action) {
				//if (text.text.size() < text.maxChar) {
				SetText(text.substr(0, atChar) + '\n' + text.substr(atChar));
				atChar++;
				//}
			}
		}
		else {
			if (action) {
				char cha = GetChar(key, elemShiftL || elemShiftR, elemAltR);
				if (cha != 0) {
					//if (text.text.size() < text.maxChar) {
					SetText(text.substr(0, atChar) + cha + text.substr(atChar));
					atChar++;
					//}
				}
			}
		}
	}

	int Text::CharOnLine(int c)
	{
		int n = 0;
		for (int i = 0; i < c; i++) {
			if (text.at(i) == '\n') {
				n = 0;
				continue;
			}
			n++;
		}
		return n;
	}
	void Text::ElemWH(float* w, float* h)
	{
		elemW = w;
		elemH = h;
	}
	void Text::SetText(const std::string& atext)
	{
		text = atext;
		/*

		unsigned int* indes = new unsigned int[6 * maxChar];
		float* verts = new float[4 * 4 * maxChar];

		float atX = 0;
		float atY = 0;
		float ratio = charHeight /(Wid()/ Hei());

		int i=0;
		for (char cha : text) {
			indes[0 + 6 * i] = 2 + 4 * i;
			indes[1 + 6 * i] = 1 + 4 * i;
			indes[2 + 6 * i] = 0 + 4 * i;
			indes[3 + 6 * i] = 0 + 4 * i;
			indes[4 + 6 * i] = 3 + 4 * i;
			indes[5 + 6 * i] = 2 + 4 * i;

			int charInd = cha;
			if (cha == '\n') {
				atY -= charHeight;
				atX = 0;
				i++;
				continue;
			}
			if (charInd < 0)
				charInd += 256;
			float wp = font->charWid[charInd] / (float)font->imgSize;
			float wi = ratio*(font->charWid[charInd]/(float)font->charSize);
			float xp = (charInd % 16);
			float yp = 15-(charInd / 16);

			Insert4(verts,   16 * i, atX, atY, (xp) / 16, (yp) / 16);
			Insert4(verts, 4+16 * i, atX, atY+ charHeight, (xp) / 16, (yp+1) / 16);
			Insert4(verts, 8+16 * i, atX+wi, atY+ charHeight, (xp) / 16+wp, (yp+1) / 16);
			Insert4(verts,12+16 * i, atX+wi, atY, (xp) / 16+wp, (yp) / 16);

			atX += wi;// +charSpacing;

			i++;
		}
		for (int i = text.length(); i < maxChar; i++) {
			indes[0 + 6 * i] = 0;
			indes[1 + 6 * i] = 0;
			indes[2 + 6 * i] = 0;
			indes[3 + 6 * i] = 0;
			indes[4 + 6 * i] = 0;
			indes[5 + 6 * i] = 0;
			for (int j = 0; j < 16;j++) {
				verts[j + 16 * i] = 0;
			}
		}

		container.SubVB(0,4*4*maxChar,verts);
		container.SubIB(0, 6 * maxChar,indes);

		//std::cout << charHeight <<" "<< (ratio / 16) << std::endl;

		float v[]{
			0,0,0,0,
			0,charHeight,0,0,
			ratio/16,charHeight,0,0,
			ratio/16,0,0,0
		};

		marker.SubVB(0, 4 * 4, v);

		delete verts;
		delete indes;*/
	}
	void Text::DrawString(float alpha, bool f)
	{
		// Dynamic
		float actualHeight = ToFloatScreenH(charHeight);
		if (!staticHeight) {
			actualHeight = ((*elemW / 2 * Width()) / PixelWidth(true)) * ToFloatScreenH(64);
			float a = *elemH / PixelHeight(false);
			if (a < 1)
				actualHeight *= a;
		}
		float x = xpos - *elemW / 2;
		float y = ypos - actualHeight + *elemH / 2;
		/*if (center) {
			x = xpos - PixelWidth(false) / 2;
			y = ypos - charHeight + PixelHeight(false) / 2;
		}*/

		SetTransform(xpos, ypos);
		SetColor(r, g, b, a * alpha);
		if (f) {
			engone::DrawString(font, text, center, actualHeight, atChar);
		}
		else {
			engone::DrawString(font, text, center, actualHeight, -1);
		}
		/*
		if (f) {
			//std::cout << (x + PixelPosX(atChar)) << " " << (y + PixelPosY(atChar)) << std::endl;
			GuiTransform(x+ScreenPosX(atChar), y-ScreenPosY(atChar));
			GuiColor(r, g, b, a);
			BindTexture("blank");
			marker.Draw();
		}*/
	}
#endif

	class Component
	{
	public:
		Component() = default;
		virtual ~Component() {}

		virtual void OnRender() {}
		virtual void OnUpdate() {}
		virtual EventType OnEvent(Event& e) { return EventType::None; }

		// owner of component
		IElement* element;
	};
	class IElement
	{
	public:
		IElement(const std::string& name, int priority=0);
		~IElement();

		std::string name;
		int priority = 0;
		ConstraintX x;
		ConstraintY y;
		ConstraintW w;
		ConstraintH h;
		float rr, rg, rb, ra;
		float renderX, renderY, renderW, renderH;

		IElement* CenterX(int pos, IElement* stick = nullptr);
		IElement* CenterX(float pos, IElement* stick = nullptr);
		IElement* Left(int pos, IElement* stick = nullptr);
		IElement* Left(float pos, IElement* stick = nullptr);
		IElement* Right(int pos, IElement* stick = nullptr);
		IElement* Right(float pos, IElement* stick = nullptr);
		IElement* CenterY(int pos, IElement* stick = nullptr);
		IElement* CenterY(float pos, IElement* stick = nullptr);
		IElement* Bottom(int pos, IElement* stick = nullptr);
		IElement* Bottom(float pos, IElement* stick = nullptr);
		IElement* Top(int pos, IElement* stick = nullptr);
		IElement* Top(float pos, IElement* stick = nullptr);
		IElement* Width(int pos, IElement* stick = nullptr);
		IElement* Width(float pos, IElement* stick = nullptr);
		IElement* Height(int pos, IElement* stick = nullptr);
		IElement* Height(float pos, IElement* stick = nullptr);

		Color color;
		IElement* Color(Color color);
		Texture* texture;
		IElement* Texture(Texture* texture);
		
		float aspectRatio=0.f;
		// @ratio Width/Height if you are using a texture
		IElement* Fixed(float ratio);

		void add(Component* component);
		Transition& add(const std::string& name);
		void add(IElement* element);

		void setTransition(const std::string& name, bool f);
		
		void OnRender();
		void OnUpdate(float delta);
		bool Inside(float mx, float my);
		EventType OnEvent(Event& e);

	protected:
		std::vector<IElement*> children;
		std::vector<Component*> components;
		std::vector<Transition> transitions;
	};
	class Button : public Component
	{
	public:
		Button();
		Button(std::function<EventType(Event& e)> func);

		EventType OnEvent(Event& e) override;

	protected:
		std::function<EventType(Event& e)> run;
	};
	class Text : public Component
	{
	public:
		Text();
		Text(const std::string& text, Font* font);
		Text(const std::string& text, Font* font, bool centered);
		Text(const std::string& text, Font* font, bool centered, float height);
		Text(const std::string& text, Font* font, bool centered, bool editable);

		std::string text;
		Font* font;

		bool center = false;
		float height = 0;

		EventType OnEvent(Event& e) override;
		void OnRender() override;

		int cursorPos = 0;
		bool isEditable, isEditing;
	};
	// Grid class is incomplete
	class Grid : public Component
	{
	public:
		Grid(int column, int row);
		Grid(int column, int row, std::function<EventType(int, Event&)> func);

		std::vector<Texture*> items;
		int column, row;
		std::function<EventType(int, Event&)> run;

		void AddItem(Texture* item);
		EventType OnEvent(Event& e) override;
		void OnRender() override;
	};

		// Set guiShader, add listener for elements
		void InitGui();

		void AddElement(IElement* element);
		void RemoveElement(const std::string& name);
		void RemoveElement(IElement* element);
		IElement* GetElement(const std::string& name);
		// Set uWindow uniform, switch blend, render elements
		void RenderElements();
		// Updates elements
		void UpdateElements(float delta);

}
