#pragma once

#include "../EventManager.h"

#include "../Handlers/AssetManager.h"
#include "../Rendering/Texture.h"

namespace engone
{
	class Panel;

	class IElement;
	enum class IPropType
	{
		X, Y, W, H, Color, Texture
	};
	class IProperty
	{
	public:
		IProperty(IElement* element);
		static const IPropType type;
		IElement* element;
	};
	class IColor : public IProperty
	{
	public:
		IColor(IElement* element);
		static const IPropType type= IPropType::Color;

		float r=1, g=1, b=1, a=1;

		void Set(float gray, float alpha = 1);
		void Set(int gray, int alpha = 255);
		void Set(float red, float green, float blue, float alpha = 1);
		void Set(int red, int green, int blue, int alpha = 255);
	};
	class ITexture : public IProperty
	{
	public:
		ITexture(IElement* element);
		static const IPropType type = IPropType::Color;

		Texture* texture;

		void Set(Texture* texture);
	};
	class IX : public IProperty
	{
	public:
		IX(IElement* element);
		static const IPropType type = IPropType::X;

		float value;
		bool fixed;
		char side;
		IElement* attached;

		IX* Center(float value, bool fixed = true, IElement* stick = nullptr);
		IX* Left(float value, bool fixed = true, IElement* stick = nullptr);
		IX* Right(float value, bool fixed = true, IElement* stick = nullptr);
		float Value();
	};
	class IY : public IProperty
	{
	public:
		IY(IElement* element);
		static const IPropType type = IPropType::Y;

		float value;
		bool fixed;
		char side;
		IElement* attached;

		IY* Center(float value, bool fixed = true, IElement* stick = nullptr);
		IY* Bottom(float value, bool fixed = true, IElement* stick = nullptr);
		IY* Top(float value, bool fixed = true, IElement* stick = nullptr);
		float Value();
	};
	class IW : public IProperty
	{
	public:
		IW(IElement* element);
		static const IPropType type = IPropType::W;

		float value;
		bool fixed;
		IElement* attached;

		IW* Center(float value, bool fixed = true, IElement* stick = nullptr);
		float Value();
	};
	class IH : public IProperty
	{
	public:
		IH(IElement* element);
		static const IPropType type = IPropType::H;

		float value;
		bool fixed;
		IElement* attached;

		IH* Center(float value, bool fixed = true, IElement* stick = nullptr);
		float Value();
	};
	class IElement
	{
	public:
		IElement(const std::string& name);
		std::string name;
		float finalX, finalY, finalW, finalH;
		int priority;

		template <class T>
		T* Property();

		bool _OnEvent(Event& e);
		virtual bool OnEvent(Event& e) { return false; }
		void _OnUpdate(float delta);
		virtual void OnUpdate(float delta) {}
		void _OnRender();
		virtual void OnRender() {}

	protected:
		std::unordered_map<IPropType, IProperty*> properties;
		std::vector<IElement*> children;
	};
	class IPanel : IElement
	{
	public:
		IPanel(const std::string& name);

		virtual void OnRender() override;

	};
	class IButton : IElement
	{
	public:
		IButton(const std::string& name);

		std::function<void(Event& e)> function;
		virtual bool OnEvent(Event& e);

		virtual void OnRender();

	};
	void AddElement(IElement* element);
	IElement* GetElement(const std::string& name);
	void UpdateUI(float delta);
	void RenderUI();


	class ConstraintX
	{
	public:
		ConstraintX(Panel* p) :panel(p)
		{

		}
		Panel* Center(int pos, bool pixelated=true, Panel* stick = nullptr)
		{
			raw = 0;
			pixelSpace = pixelated;
			parent = stick;
			return panel;
		}
		Panel* Left(int pos, bool pixelated = true, Panel* stick = nullptr)
		{
			side = -1;
			raw = pos;
			pixelSpace = pixelated;
			parent = stick;
			return panel;
		}
		Panel* Right(int pos, bool pixelated = true, Panel* stick = nullptr)
		{
			side = 1;
			raw = pos;
			pixelSpace = pixelated;
			parent = stick;
			return panel;
		}
		float Value();
	private:
		bool pixelSpace;
		float raw = 0;
		Panel* panel = nullptr;
		Panel* parent = nullptr;
		int side = 0;
	};
	class ConstraintY
	{
	public:
		ConstraintY(Panel* p) :panel(p)
		{

		}
		Panel* Center(int pos, bool pixelated = true, Panel* stick = nullptr)
		{
			side = 0;
			raw = pos;
			pixelSpace = pixelated;
			parent = stick;
			return panel;
		}
		Panel* Bottom(int pos, bool pixelated = true, Panel* stick = nullptr)
		{
			side = -1;
			raw = pos;
			pixelSpace = pixelated;
			parent = stick;
			return panel;
		}
		Panel* Top(int pos, bool pixelated = true, Panel* stick = nullptr)
		{
			side = 1;
			pixelSpace = pixelated;
			raw = pos;
			parent = stick;
			return panel;
		}
		float Value();
	private:
		bool pixelSpace;
		float raw = 0;
		Panel* panel = nullptr;
		Panel* parent = nullptr;
		int side = 0;
	};
	class ConstraintW
	{
	public:
		ConstraintW(Panel* p) :panel(p)
		{

		}
		Panel* Center(int pos, bool pixelated = true, Panel* stick = nullptr)
		{
			raw = pos;
			pixelSpace = pixelated;
			parent = stick;
			return panel;
		}
		float Value();
	private:
		bool pixelSpace;
		float raw = 0;
		Panel* panel = nullptr;
		Panel* parent = nullptr;
	};
	class ConstraintH
	{
	public:
		ConstraintH(Panel* p) : panel(p)
		{

		}
		Panel* Center(int pos, bool pixelated = true, Panel* stick = nullptr)
		{
			raw = pos;
			pixelSpace = pixelated;
			parent = stick;
			return panel;
		}
		float Value();
	private:
		bool pixelSpace;
		float raw = 0;
		Panel* panel = nullptr;
		Panel* parent = nullptr;
	};
	class Color
	{
	public:
		float r = 1, g = 1, b = 1, a = 1;
		Color() = default;
		Color(float rgba) : r(1), g(1), b(1), a(1) {}
		Color(int rgba)
			: r(rgba / 256.f), g(rgba / 256.f), b(rgba / 256.f), a(rgba / 256.f)
		{
		}
		Color(float rgb, float alpha)
			: r(rgb), g(rgb), b(rgb), a(alpha)
		{
		}
		Color(int rgb, int alpha)
			: r(rgb), g(rgb), b(rgb), a(alpha)
		{
		}
		Color(float red, float green, float blue)
			: r(red), g(green), b(blue), a(1)
		{
		}
		Color(int red, int green, int blue)
			: r(red / 256.f), g(green / 256.f), b(blue / 256.f), a(1)
		{
		}
		Color(float red, float green, float blue, float alpha)
			: r(red), g(green), b(blue), a(alpha)
		{
		}
		Color(int red, int green, int blue, int alpha)
			: r(red / 256.f), g(green / 256.f), b(blue / 256.f), a(alpha / 256.f)
		{
		}
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

		Transition(const std::string& name) :name(name) {};

		std::string name;

		EventType eventType;

		bool active = false;
		void Active(bool f)
		{
			active = f;
		}
		Transition* Fade(const Color& c, float time)
		{
			fade = true;
			color = c;
			fadeS = 1 / time;
			return this;
		}
		Transition* Move(int f0, int f1, float time)
		{
			move = true;
			x = f0;
			y = f1;
			moveS = 1 / time;
			return this;
		}
		Transition* Size(int f0, int f1, float time)
		{
			size = true;
			w = f0;
			h = f1;
			sizeS = 1 / time;
			return this;
		}

		float ValueX()
		{
			return x * moveT;
		}
		float ValueY()
		{
			return y * moveT;
		}
		float ValueW()
		{
			return w * sizeT;
		}
		float ValueH()
		{
			return h * sizeT;
		}

		// Fade is calculated in element.cpp since it requires some more hardcore calculations
		// Might be possible to make a functions out of it but ehh, whatever

		void Update(float delta)
		{
			if (fade) {
				if (active) {
					fadeT += fadeS * delta;
				}
				else {
					fadeT -= fadeS * delta;
				}

				if (fadeT < 0) {
					fadeT = 0;
				}
				else if (fadeT > 1) {
					fadeT = 1;
				}
			}
			if (move) {
				if (active) {
					moveT += moveS * delta;
				}
				else {
					moveT -= moveS * delta;
				}
				if (moveT < 0) {
					moveT = 0;
				}
				else if (moveT > 1) {
					moveT = 1;
				}
			}
			if (size) {
				if (active) {
					sizeT += sizeS * delta;
				}
				else {
					sizeT -= sizeS * delta;
				}
				if (sizeT < 0) {
					sizeT = 0;
				}
				else if (sizeT > 1) {
					sizeT = 1;
				}
			}
		}
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

	//Shader& GetShader();

	class Component
	{
	public:
		Component() = default;

		virtual void OnRender() {}
		virtual void OnUpdate() {}
		virtual bool OnEvent(Event& e) { return false; }

		Panel* panel;

	};

	class Panel
	{
	public:
		Panel(const std::string& name)
			: x(this), y(this), w(this), h(this), name(name)
		{
			Init();
		}
		std::string name;
		float priority = 0;
		ConstraintX x;
		ConstraintY y;
		ConstraintW w;
		ConstraintH h;

		Panel* CenterX(int pos, Panel* stick = nullptr)
		{
			y.Center(pos, stick);
			return this;
		}
		Panel* CenterX(float pos, Panel* stick = nullptr)
		{
			y.Center(pos, stick);
			return this;
		}
		Panel* Bottom(int pos, Panel* stick = nullptr)
		{
			y.Bottom(pos, stick);
			return this;
		}
		Panel* Top(int pos, Panel* stick = nullptr)
		{
			y.Top(pos, stick);
			return this;
		}
		Panel* CenterY(int pos, Panel* stick = nullptr)
		{
			x.Center(pos, stick);
			return this;
		}
		Panel* Left(int pos, Panel* stick = nullptr)
		{
			x.Left(pos, stick);
			return this;
		}
		Panel* Right(int pos, Panel* stick = nullptr)
		{
			x.Right(pos, stick);
			return this;
		}
		Panel* Width(int pos, Panel* stick = nullptr)
		{
			w.Center(pos, stick);
			return this;
		}
		Panel* Height(int pos, Panel* stick = nullptr)
		{
			h.Center(pos, stick);
			return this;
		}
		float rr, rg, rb, ra;
		float renderX, renderY, renderW, renderH;

		Color color;
		Panel* Color(Color color)
		{
			this->color = color;
			return this;
		}
		Texture* texture;

		void AddComponent(Component* component)
		{
			component->panel = this;
			components.push_back(component);
		}/*
		Component* GetComponent(int index)
		{
			if (index < 0 || index >= components.size())
				return nullptr;
			return components[index];
		*/
		void SetTransition(const std::string& name, bool f)
		{
			for (int i = 0; i < transitions.size(); i++) {
				if (transitions[i].name == name) {
					transitions[i].active = f;
				}
			}
		}
		Transition& AddTransition(const std::string& name)
		{
			transitions.push_back({ name });
			return transitions.back();
		}
		void Init()
		{
			/*
			float vert[]{
				0,0,0,0,
				1,0,1,0,
				1,1,1,1,
				0,1,0,1,
			};
			unsigned int index[]{
				0,1,2,
				2,3,0
			};

			buffer.Init(false,vert,16,index,6);
			buffer.SetAttrib(0, 4, 4, 0);
			*/
		}
		void OnRender()
		{
			if (ra != 0) { // panel is transparent
				Shader* gui = GetAsset<Shader>("gui");
				gui->SetVec2("uSize", { renderW, renderH });
				gui->SetVec2("uPos", { renderX, renderY });
				gui->SetVec4("uColor", rr, rg, rb, ra);

				if (texture != nullptr) {
					texture->Bind();
					gui->SetInt("uTextured", 1);
				}
				else
					gui->SetInt("uTextured", 0);

				DrawRect();
			}
			for (int i = 0; i < components.size(); i++) {
				components[i]->OnRender();
			}
			for (int i = 0; i < children.size(); i++) {
				children[i]->OnRender();
			}
		}
		void OnUpdate(float delta)
		{
			float redBase = color.r;
			float greenBase = color.g;
			float blueBase = color.b;
			float alphaBase = color.a;
			int fadeCount = 0;
			for (auto& t : transitions) {
				if (t.eventType == EventType::Move) {
					t.active = Inside(GetMouseX(), GetMouseY());
				}
				t.Update(delta);
				if (t.fade) {
					redBase *= (1 - t.fadeT);
					greenBase *= (1 - t.fadeT);
					blueBase *= (1 - t.fadeT);
					alphaBase *= (1 - t.fadeT);
					fadeCount++;
				}
			}
			renderW = w.Value(); renderH = h.Value();
			renderX = x.Value(); renderY = y.Value();

			rr = 0;
			rg = 0;
			rb = 0;
			ra = 0;
			rr += redBase;
			rg += greenBase;
			rb += blueBase;
			ra += alphaBase;
			for (auto& t : transitions) {
				if (t.fade) {
					redBase = t.color.r * t.fadeT;
					greenBase = t.color.g * t.fadeT;
					blueBase = t.color.b * t.fadeT;
					alphaBase = t.color.a * t.fadeT;
					for (auto& t2 : transitions) {
						if (&t == &t2 || !t2.fade)
							continue;
						redBase *= (1 - t2.fadeT / fadeCount);
						greenBase *= (1 - t2.fadeT / fadeCount);
						blueBase *= (1 - t2.fadeT / fadeCount);
						alphaBase *= (1 - t2.fadeT / fadeCount);

					}
					rr += redBase;
					rg += greenBase;
					rb += blueBase;
					ra += alphaBase;
				}
				//std::cout << t.move<<" move\n";
				if (t.move) {
					renderX += t.ValueX();
					renderY += t.ValueY();
				}
				if (t.size) {
					renderW += t.ValueW();
					renderH += t.ValueH();
				}
			}
			for (int i = 0; i < components.size(); i++) {
				components[i]->OnUpdate();
			}
			for (int i = 0; i < children.size(); i++) {
				children[i]->OnUpdate(delta);
			}
		}
		bool Inside(float mx, float my)
		{
			//std::cout << rx << " " << ry << " " << rw << " " << rh << " "<<mx<<" " <<my<<"\n";
			return mx > renderX && mx<renderX + renderW && my>renderY && my < renderY + renderH;
		}
		bool OnEvent(Event& e)
		{
			if (e.eventType == (EventType::Click)) {
				bool inside = Inside(e.mx, e.my);
				for (int i = 0; i < transitions.size(); i++) {
					if (transitions[i].eventType == e.eventType) {
						transitions[i].active = inside;
					}
				}
			}
			for (int i = 0; i < components.size(); i++) {
				if (components[i]->OnEvent(e))
					return true;
			}
			return false;
		}


	protected:
		//TriangleBuffer buffer;
		std::vector<Panel*> children;
		std::vector<Component*> components;
		std::vector<Transition> transitions;
	};
	class Button : public Component
	{
	public:
		Button() : Component() {}
		Button(std::function<bool(Event& e)> func) : Component(), run(func) {}

		std::function<bool(Event& e)> run;
		virtual bool OnEvent(Event& e) override
		{
			if (e.eventType == (EventType::Click)) {
				if (panel->Inside(e.mx, e.my) && run != nullptr)
					if (run(e))
						return true;
			}
			return false;
		}
	};
	class Text : public Component
	{
	public:
		Text() = default;
		Text(const std::string& text, Font* font)
			: text(text), font(font)
		{
		}
		Text(const std::string& text, Font* font, bool centered)
			: text(text), font(font), center(centered)
		{
		}
		Text(const std::string& text, Font* font, bool centered, float height)
			: text(text), font(font), center(centered), height(height)
		{
		}
		Text(const std::string& text, Font* font, bool centered, bool editable)
			: text(text), font(font), center(centered), isEditable(editable)
		{
		}

		std::string text;
		Font* font;

		bool center = false;
		float height = 0;

		virtual bool OnEvent(Event& e) override
		{
			if (isEditable) {
				if (e.eventType == EventType::Click) {
					if (e.action == 1) {
						if (panel->Inside(e.mx, e.my)) {
							isEditing = true;
							return true;
						}
						else {
							isEditing = false;
							return false;
						}
					}
				}
			}
			if (isEditing) {
				if (e.eventType == EventType::Key) {
					if (e.key == GLFW_KEY_LEFT_SHIFT) {
						//elemShiftL = action;
					}
					if (e.key == GLFW_KEY_RIGHT_SHIFT) {
						//elemShiftR = action;
					}
					else if (e.key == GLFW_KEY_RIGHT_ALT) {
						//elemAltR = action;
					}
					else if (e.key == GLFW_KEY_UP) {
						if (e.action == 1) {
							int c = cursorPos;
							int l = 0;// CharOnLine(c);
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
							cursorPos = newC;
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
					else if (e.key == GLFW_KEY_LEFT) {
						if (e.action == 1) {
							if (cursorPos > 0)
								cursorPos--;
						}
					}
					else if (e.key == GLFW_KEY_DOWN) {
						if (e.action == 1) {
							int c = cursorPos;
							int l = 0;// CharOnLine(c);
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
							cursorPos = newC;

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
					else if (e.key == GLFW_KEY_RIGHT) {
						if (e.action == 1) {
							if (cursorPos < text.size())
								cursorPos++;
						}
					}
					else if (e.key == GLFW_KEY_DELETE) {
						if (e.action == 1) {
							if (text.size() > 0 && cursorPos < text.size()) {
								if (cursorPos == 0) {
									text = text.substr(1);
								}
								else {
									text = text.substr(0, cursorPos) + text.substr(cursorPos + 1);
								}
							}
						}
					}
					else if (e.key == GLFW_KEY_BACKSPACE) {
						if (e.action == 1) {
							if (text.length() > 0 && cursorPos > 0) {
								if (cursorPos == text.size()) {
									text = text.substr(0, cursorPos - 1);
									cursorPos--;
								}
								else {
									text = text.substr(0, cursorPos - 1) + text.substr(cursorPos);
									cursorPos--;
								}
							}
						}
					}
					else if (e.key == GLFW_KEY_ENTER) {
						if (e.action == 1) {
							//if (text.text.size() < text.maxChar) {
							text = text.substr(0, cursorPos) + '\n' + text.substr(cursorPos);
							cursorPos++;
							//std::cout <<text<<"]\n";
							//}
						}
					}
					else {
						if (e.action == 1) {
							char chr = (char)e.key;// GetChar(key, elemShiftL || elemShiftR, elemAltR);
							//std::cout << "Hellu24! " << chr << (char)e.key<< "\n";
							if (chr != 0) {
								//if (text.text.size() < text.maxChar) {
								text = text.substr(0, cursorPos) + chr + text.substr(cursorPos);
								cursorPos++;
								//}
							}
						}
					}
				}
			}
			return false;
		}
		virtual void OnRender() override
		{
			Shader* shad = GetAsset<Shader>("gui");
			shad->Bind();
			//std::cout << shad<<"\n";
			shad->SetVec2("uPos", { panel->renderX,panel->renderY });
			shad->SetVec2("uSize", { 1,1 });
			shad->SetVec4("uColor", 0.5, 1, .2, 1);
			shad->SetInt("uTextured", 1);

			int hej = -1;
			if (isEditing)
				hej = cursorPos;
			if (height == 0)
				DrawString(font, text, center, panel->renderH, panel->renderW, panel->renderH, hej);
			else
				DrawString(font, text, center, height, panel->renderW, panel->renderH, hej);
		}

		int cursorPos = 0;
		bool isEditable, isEditing;
	};
	class Grid : public Component
	{
	public:
		Grid(int column, int row) : column(column), row(row) {}
		Grid(int column, int row, std::function<bool(int, Event&)> func) : column(column), row(row), run(func) {}

		std::vector<Texture*> items;
		int column, row;
		std::function<bool(int, Event&)> run;

		void AddItem(Texture* item)
		{
			items.push_back(item);
		}

		virtual bool OnEvent(Event& e) override
		{
			if (run == nullptr)
				return false;
			if (panel->Inside(e.mx, e.my)) {
				if (column != 0) {
					int size = panel->renderW / column;
					int ix = (e.mx - panel->renderX) / size;
					int iy = (e.my - panel->renderY) / size;

					int index = iy * column + ix;
					//std::cout << index << "\n";
					if (index > -1 && index < items.size()) {
						if (run(index, e))
							return true;
					}
				}
				else {
					int size = size = panel->renderH / row;
					int ix = (e.mx - panel->renderX) / size;
					int iy = (e.my - panel->renderY) / size;

					int index = ix * column + iy;
					//std::cout << index << "\n";
					if (index > -1 && index < items.size()) {
						if (run(index, e))
							return true;
					}
				}
			}
			return false;
		}
		virtual void OnRender() override
		{
			Shader* shad = GetAsset<Shader>("gui");
			shad->SetInt("uTextured", 1);
			int size;
			if (column != 0) {
				size = panel->renderW / column;
				shad->SetVec2("uSize", { size,size });
				for (int i = 0; i < items.size(); i++) {
					int x = panel->renderX + (i % column) * size;
					int y = panel->renderY + (i / column) * size;
					shad->SetVec2("uPos", { x, y });
					items[i]->Bind();
					DrawRect();
				}
			}
			else {
				size = panel->renderH / row;
				shad->SetVec2("uSize", { size,size });
				for (int i = 0; i < items.size(); i++) {
					int x = panel->renderX + (i / row) * size;
					int y = panel->renderY + (i % row) * size;
					shad->SetVec2("uPos", { x, y });
					items[i]->Bind();
					DrawRect();
				}
			}
		}
	};
	void AddPanel(Panel* panel);
	Panel* GetPanel(const std::string& name);
	void InitGUI();
	void RenderPanels();
	void UpdatePanels(float delta);
}
//#endif