#include "gonpch.h"

#include "GuiHandler.h"

#include "../Window.h"

namespace engone
{
	static Shader* guiShader;

	ConstraintX::ConstraintX(IElement* p) :element(p){}
	IElement* ConstraintX::ConstraintX::Center(float pos, bool pixelated, IElement* stick)
	{
		side = 0;
		raw = pos;
		pixelSpace = pixelated;
		parent = stick;
		return element;
	}
	IElement* ConstraintX::Left(float pos, bool pixelated, IElement* stick)
	{
		side = -1;
		raw = pos;
		pixelSpace = pixelated;
		parent = stick;
		return element;
	}
	IElement* ConstraintX::Right(float pos, bool pixelated, IElement* stick)
	{
		side = 1;
		raw = pos;
		pixelSpace = pixelated;
		parent = stick;
		return element;
	}
	float ConstraintX::Value()
	{
		float move = raw;
		if (!pixelSpace) {
			move *= GetWidth();
		}

		if (side == 0) {
			if (parent == nullptr) return move - element->renderW / 2 + GetWidth() / 2;
			else return parent->renderX + move;
		}
		else if (side == 1) {
			if (parent == nullptr) return GetWidth() - move - element->renderW;
			else return parent->renderX - move - element->renderW;
		}
		else {
			if (parent == nullptr) return move;
			else return parent->renderX + parent->renderW + move;
		}
	}

	ConstraintY::ConstraintY(IElement* p) :element(p){}
	IElement* ConstraintY::Center(float pos, bool pixelated, IElement* stick)
	{
		side = 0;
		raw = pos;
		pixelSpace = pixelated;
		parent = stick;
		return element;
	}
	IElement* ConstraintY::Bottom(float pos, bool pixelated, IElement* stick)
	{
		side = -1;
		raw = pos;
		pixelSpace = pixelated;
		parent = stick;
		return element;
	}
	IElement* ConstraintY::Top(float pos, bool pixelated, IElement* stick)
	{
		side = 1;
		raw = pos;
		pixelSpace = pixelated;
		parent = stick;
		return element;
	}
	float ConstraintY::Value()
	{
		float move = raw;
		if (!pixelSpace) {
			move *= GetHeight();
		}

		if (side == 0) {
			if (parent == nullptr) return move - element->renderH / 2 + GetHeight() / 2;
			else return parent->renderY + move;
		}
		else if (side == 1) {
			if (parent == nullptr) return move;// - panel->renderH;
			else return parent->renderY + move + parent->renderH;
		}
		else {
			if (parent == nullptr) return GetHeight() - move - element->renderH;
			else return parent->renderY - element->renderH - move;
		}
	}

	ConstraintW::ConstraintW(IElement* p) :element(p){}
	IElement* ConstraintW::Center(float pos, bool pixelated, IElement* stick)
	{
		raw = pos;
		pixelSpace = pixelated;
		parent = stick;
		return element;
	}
	float ConstraintW::Value()
	{
		float move = raw;
		if (!pixelSpace) {
			move *= GetWidth();
		}

		if (parent == nullptr) return move;
		else return element->renderW + move;
	}

	ConstraintH::ConstraintH(IElement* p) : element(p){}
	IElement* ConstraintH::Center(float pos, bool pixelated, IElement* stick)
	{
		raw = pos;
		pixelSpace = pixelated;
		parent = stick;
		return element;
	}
	float ConstraintH::Value()
	{
		float move = raw;
		if (!pixelSpace) {
			move *= GetHeight();
		}

		if (parent == nullptr) return move;
		else return parent->renderH + move;
	}

	Color::Color() : r(1), g(1), b(1), a(1) {};
	Color::Color(float rgb) 
		: r(rgb), g(rgb), b(rgb), a(1) {}
	Color::Color(int rgb)
		: r(rgb / 256.f), g(rgb / 256.f), b(rgb / 256.f), a(1) {}
	Color::Color(float rgb, float alpha)
		: r(rgb), g(rgb), b(rgb), a(alpha) {}
	Color::Color(int rgb, int alpha)
		: r(rgb), g(rgb), b(rgb), a(alpha) {}
	Color::Color(float red, float green, float blue)
		: r(red), g(green), b(blue), a(1) {}
	Color::Color(int red, int green, int blue)
		: r(red / 256.f), g(green / 256.f), b(blue / 256.f), a(1) {}
	Color::Color(float red, float green, float blue, float alpha)
		: r(red), g(green), b(blue), a(alpha) {}
	Color::Color(int red, int green, int blue, int alpha)
		: r(red / 256.f), g(green / 256.f), b(blue / 256.f), a(alpha / 256.f) {}

	Transition::Transition(const std::string& name) : name(name) {}
	void Transition::Active(bool f) {
		active = f;
	}
	Transition* Transition::Fade(const Color& c, float time) {
		fade = true;
		color = c;
		fadeS = 1 / time;
		return this;
	}
	Transition* Transition::Move(float x, float y, float time) {
		move = true;
		this->x = x;
		this->y = y;
		moveS = 1 / time;
		return this;
	}
	Transition* Transition::Size(float w, float h, float time) {
		size = true;
		this->w = w;
		this->h = h;
		sizeS = 1 / time;
		return this;
	}
	float Transition::ValueX() {
		return x * moveT;
	}
	float Transition::ValueY() {
		return y * moveT;
	}
	float Transition::ValueW() {
		return w * sizeT;
	}
	float Transition::ValueH() {
		return h * sizeT;
	}
	void Transition::Update(float delta) {
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

	IElement::IElement(const std::string& name, int priority)
		: x(this), y(this), w(this), h(this), name(name), priority(priority) {}
	IElement::~IElement() {
		for (int i = 0; i < children.size(); i++) {
			delete children[i];
		}
		for (int i = 0; i < panels.size(); i++) {
			delete panels[i];
		}
	}
	IElement* IElement::CenterX(int pos, IElement* stick) {
		y.Center(pos, true, stick);
		return this;
	}
	IElement* IElement::CenterX(float pos, IElement* stick) {
		y.Center(pos, false, stick);
		return this;
	}
	IElement* IElement::Left(int pos, IElement* stick) {
		x.Left(pos, true, stick);
		return this;
	}
	IElement* IElement::Left(float pos, IElement* stick) {
		x.Left(pos, false, stick);
		return this;
	}
	IElement* IElement::Right(int pos, IElement* stick) {
		x.Right(pos, true, stick);
		return this;
	}
	IElement* IElement::Right(float pos, IElement* stick) {
		x.Right(pos, false, stick);
		return this;
	}
	IElement* IElement::CenterY(int pos, IElement* stick) {
		x.Center(pos, true, stick);
		return this;
	}
	IElement* IElement::CenterY(float pos, IElement* stick) {
		x.Center(pos, false, stick);
		return this;
	}
	IElement* IElement::Bottom(int pos, IElement* stick) {
		y.Bottom(pos, true, stick);
		return this;
	}
	IElement* IElement::Bottom(float pos, IElement* stick) {
		y.Bottom(pos, false, stick);
		return this;
	}
	IElement* IElement::Top(int pos, IElement* stick) {
		y.Top(pos, true, stick);
		return this;
	}
	IElement* IElement::Top(float pos, IElement* stick) {
		y.Top(pos, false, stick);
		return this;
	}
	IElement* IElement::Width(int pos, IElement* stick) {
		w.Center(pos, true, stick);
		return this;
	}
	IElement* IElement::Width(float pos, IElement* stick) {
		w.Center(pos, false, stick);
		return this;
	}
	IElement* IElement::Height(int pos, IElement* stick) {
		h.Center(pos, true, stick);
		return this;
	}
	IElement* IElement::Height(float pos, IElement* stick) {
		h.Center(pos, false, stick);
		return this;
	}
	IElement* IElement::Color(engone::Color color) {
		this->color = color;
		return this;
	}
	IElement* IElement::Texture(engone::Texture* texture) {
		this->texture = texture;
		return this;
	}
	IElement* IElement::Fixed(float ratio) {
		aspectRatio = ratio;
		return this;
	}
	void IElement::add(Panel* component) {
		component->element = this;
		panels.push_back(component);
	}
	Transition& IElement::add(const std::string& name) {
		transitions.push_back({ name });
		return transitions.back();
	}
	void IElement::add(IElement* element) {
		children.push_back(element);
	}
	void IElement::setTransition(const std::string& name, bool f) {
		for (int i = 0; i < transitions.size(); i++) {
			if (transitions[i].name == name) {
				transitions[i].active = f;
			}
		}
	}
	void IElement::OnRender() {
		if (ra != 0) { // element is transparent
			Shader* gui = GetAsset<Shader>("gui");
			//log::out << " " << renderW << "\n";
			gui->SetVec2("uSize", { renderW, renderH });
			gui->SetVec2("uPos", { renderX, renderY });
			gui->SetVec4("uColor", rr, rg, rb, ra);

			if (texture != nullptr) {
				texture->Bind();
				gui->SetInt("uColorMode", 2);
			}
			else {
				gui->SetInt("uColorMode", 0);
			}
			DrawRect();
		}
		for (int i = 0; i < panels.size(); i++) {
			panels[i]->OnRender();
		}
		for (int i = 0; i < children.size(); i++) {
			children[i]->OnRender();
		}
	}
	void IElement::OnUpdate(float delta) {
		float redBase = color.r;
		float greenBase = color.g;
		float blueBase = color.b;
		float alphaBase = color.a;
		int fadeCount = 0;
		for (auto& t : transitions) {
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

		if (aspectRatio != 0) {
			float ratio = renderW / renderH;
			if (ratio < aspectRatio) {
				renderH = renderW / aspectRatio;
			}
			else if (ratio > aspectRatio) {
				renderW = aspectRatio * renderH;
			}
		}

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
			if (t.move) {
				renderX += t.ValueX();
				renderY += t.ValueY();
			}
			if (t.size) {
				renderW += t.ValueW();
				renderH += t.ValueH();
			}
		}

		for (int i = 0; i < panels.size(); i++) {
			panels[i]->OnUpdate();
		}
		for (int i = 0; i < children.size(); i++) {
			children[i]->OnUpdate(delta);
		}
	}
	bool IElement::Inside(float mx, float my) {
		return mx > renderX && mx<renderX + renderW && my>renderY && my < renderY + renderH;
	}
	EventType IElement::OnEvent(Event& e) {
		if (e.eventType == EventType::Click) {
			bool inside = Inside(e.mx, e.my);
			for (int i = 0; i < transitions.size(); i++) {
				if (transitions[i].eventType == e.eventType) {
					transitions[i].active = inside;
				}
			}
		}
		for (int i = 0; i < panels.size(); i++) {
			EventType ret = panels[i]->OnEvent(e);
			if (ret != EventType::None)
				return ret;
		}
		return EventType::None;
	}

	Button::Button() : Panel() {}
	Button::Button(std::function<EventType(Event& e)> func) : Panel(), run(func) {}
	EventType Button::OnEvent(Event& e) {
		if (e.eventType == (EventType::Click)) {
			if (element->Inside(e.mx, e.my) && run != nullptr) {
				EventType ret = run(e);
				if (ret != EventType::None)
					return ret;
			}
		}
		return EventType::None;
	}

	Text::Text() = default;
	Text::Text(const std::string & text, Font * font)
		: text(text), font(font) {}
	Text::Text(const std::string & text, Font * font, bool centered)
		: text(text), font(font), center(centered) {}
	Text::Text(const std::string & text, Font * font, bool centered, float height)
		: text(text), font(font), center(centered), height(height) {}
	Text::Text(const std::string & text, Font * font, bool centered, bool editable)
		: text(text), font(font), center(centered), isEditable(editable) {}
	EventType Text::OnEvent(Event& e)  {
		if (isEditable) {
			if (e.eventType == EventType::Click) {
				if (e.action == 1) {
					if (element->Inside(e.mx, e.my)) {
						isEditing = true;
						return EventType::Click;
					}
					else {
						isEditing = false;
						return EventType::None;
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
		return EventType::None;
	}
	void Text::OnRender() {
		guiShader->Bind();
		guiShader->SetVec2("uPos", { element->renderX,element->renderY });
		guiShader->SetVec2("uSize", { 1,1 });
		guiShader->SetVec4("uColor", 0.5, 1, .2, 1);
		guiShader->SetInt("uColorMode", 1);

		int hej = -1;
		if (isEditing)
			hej = cursorPos;
		if (height == 0)
			DrawString(font, text, center, element->renderH, element->renderW, element->renderH, hej);
		else
			DrawString(font, text, center, height, element->renderW, element->renderH, hej);
	}

	Grid::Grid(int column, int row) : column(column), row(row) {}
	Grid::Grid(int column, int row, std::function<EventType(int, Event&)> func) : column(column), row(row), run(func) {}
	void Grid::AddItem(Texture* item) {
		items.push_back(item);
	}
	EventType Grid::OnEvent(Event& e) {
		if (run == nullptr)
			return EventType::None;
		if (e.eventType == EventType::Click) {
			if (element->Inside(e.mx, e.my)) {
				if (column != 0) {
					int size = element->renderW / column;
					int ix = (e.mx - element->renderX) / size;
					int iy = (e.my - element->renderY) / size;

					int index = iy * column + ix;
					//std::cout << index << "\n";
					if (index > -1 && index < items.size()) {
						EventType ret = run(index, e);
						if (ret != EventType::None) {
							return ret;
						}
					}
				}
				else {
					int size = size = element->renderH / row;
					int ix = (e.mx - element->renderX) / size;
					int iy = (e.my - element->renderY) / size;

					int index = ix * column + iy;
					//std::cout << index << "\n";
					if (index > -1 && index < items.size()) {
						EventType ret = run(index, e);
						if (ret != EventType::None) {
							return ret;
						}
					}
				}
			}
		}
		return EventType::None;
	}
	void Grid::OnRender() {
		Shader* shad = GetAsset<Shader>("gui");
		shad->SetInt("uColorMode", 2);
		int size;
		if (column != 0) {
			size = element->renderW / column;
			shad->SetVec2("uSize", { size,size });
			for (int i = 0; i < items.size(); i++) {
				int x = element->renderX + (i % column) * size;
				int y = element->renderY + (i / column) * size;
				shad->SetVec2("uPos", { x, y });
				items[i]->Bind();
				DrawRect();
			}
		}
		else {
			size = element->renderH / row;
			shad->SetVec2("uSize", { size,size });
			for (int i = 0; i < items.size(); i++) {
				int x = element->renderX + (i / row) * size;
				int y = element->renderY + (i % row) * size;
				shad->SetVec2("uPos", { x, y });
				items[i]->Bind();
				DrawRect();
			}
		}
	}
	static std::vector<IElement*> elements;
	void InitGui() {
		guiShader = GetAsset<Shader>("gui");
		AddListener(new Listener(EventType::Click | EventType::Move | EventType::Key | EventType::Scroll, [=](Event& e) {
			for (int i = 0; i < elements.size(); i++) {
				EventType ret = elements[i]->OnEvent(e);
				if (ret != EventType::None)
					return ret;
			}
			return EventType::None;
			}));
	}
	void AddElement(IElement* element)
	{
		int insertIndex = -1;
		for (int i = 0; i < elements.size(); i++) {
			if (elements[i] == element)
				return;
			if (insertIndex == -1 && element->priority > elements[i]->priority)
				insertIndex = i;
		}
		if (insertIndex == -1)
			elements.push_back(element);
		else
			elements.insert(elements.begin()+insertIndex, element);
	}
	void RemoveElement(const std::string& name)
	{
		for (int i = 0; i < elements.size();i++) {
			if (elements[i]->name == name) {
				delete elements[i];
				elements.erase(elements.begin() + i);
				return;
			}
		}
	}
	void RemoveElement(IElement* element) {
		for (int i = 0; i < elements.size(); i++) {
			if (elements[i]==element) {
				delete elements[i];
				elements.erase(elements.begin() + i);
				return;
			}
		}
	}
	IElement* GetElement(const std::string& name)
	{
		for (int i = 0; i < elements.size();i++) {
			if (elements[i]->name == name) {
				return elements[i];
			}
		}
		return nullptr;
	}
	void RenderElements()
	{
		EnableBlend();
		guiShader->Bind();
		guiShader->SetVec2("uWindow", { GetWidth(),GetHeight() });
		//log::out << "win " << Width() << "\n";
		for (int i = elements.size()-1; i > -1; i--) {
			elements[i]->OnRender();
		}
	}
	void UpdateElements(float delta)
	{
		for (int i = 0; i < elements.size(); i++) {
			elements[i]->OnUpdate(delta);
		}
	}
}