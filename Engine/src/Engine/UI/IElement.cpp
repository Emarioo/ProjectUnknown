#include "IElement.h"

namespace engine {
	Color::Color()
		: r(0), g(0), b(0), a(1) {}
	Color::Color(float f) 
		: r(f),g(f),b(f),a(1) { }
	Color::Color(int i) 
		: r(i/256.f), g(i/256.f), b(i/256.f), a(1) { }
	Color::Color(float f, float a)
		: r(f), g(f), b(f), a(a) { }
	Color::Color(int i, float a)
		: r(i / 256.f), g(i / 256.f), b(i / 256.f), a(a) { }
	Color::Color(float red, float green, float blue)
		: r(red), g(green), b(blue), a(1) { }
	Color::Color(int red, int green, int blue)
		: r(red / 256.f), g(green / 256.f), b(blue / 256.f), a(1) { }
	Color::Color(float red, float green, float blue, float alpha)
		: r(red),g(green),b(blue),a(alpha) { }
	Color::Color(int red, int green, int blue, float alpha)
		: r(red / 256.f), g(green / 256.f), b(blue / 256.f), a(alpha) { }

	IConstraint::IConstraint(IElement* e, bool use_xAxis) : parent(e), xAxis(use_xAxis) {}
	IElement* IConstraint::Center(int pos) {
		center = true;
		isPixel = true;
		raw = pos;
		return parent;
	}
	IElement* IConstraint::Center(float pos) {
		center = true;
		if (xAxis) raw = AlterSW(pos);
		else raw = AlterSH(pos);
		return parent;
	}
	IElement* IConstraint::Center(IElement* element, int pos) {
		center = true;
		elem = element;
		isPixel = true;
		raw = pos;
		return parent;
	}
	IElement* IConstraint::Center(IElement* element, float pos) {
		center = true;
		elem = element;
		if (xAxis) raw = AlterSW(pos);
		else raw = AlterSH(pos);
		return parent;
	}
	IElement* IConstraint::Left(int pos) {
		xAxis = true;
		side = false;
		isPixel = true;
		raw = pos;
		return parent;
	}
	IElement* IConstraint::Left(float pos) {
		xAxis = true;
		side = false;
		raw = AlterSW(pos);
		return parent;
	}
	IElement* IConstraint::Left(IElement* element, int pos) {
		xAxis = true;
		side = false;
		isPixel = true;
		elem = element;
		raw = pos;
		return parent;
	}
	IElement* IConstraint::Left(IElement* element, float pos) {
		xAxis = true;
		side = false;
		elem = element;
		raw = AlterSW(pos);
		return parent;
	}
	IElement* IConstraint::Right(int pos) {
		xAxis = true;
		side = true;
		isPixel = true;
		raw = pos;
		return parent;
	}
	IElement* IConstraint::Right(float pos) {
		xAxis = true;
		side = true;
		raw = AlterSW(pos);
		return parent;
	}
	IElement* IConstraint::Right(IElement* element, int pos) {
		xAxis = true;
		side = true;
		isPixel = true;
		elem = element;
		raw = pos;
		return parent;
	}
	IElement* IConstraint::Right(IElement* element, float pos) {
		xAxis = true;
		side = true;
		elem = element;
		raw = AlterSW(pos);
		return parent;
	}
	IElement* IConstraint::Bottom(int pos) {
		xAxis = false;
		side = false;
		isPixel = true;
		raw = pos;
		return parent;
	}
	IElement* IConstraint::Bottom(float pos) {
		xAxis = false;
		side = false;
		raw = AlterSH(pos);
		return parent;
	}
	IElement* IConstraint::Bottom(IElement* element, int pos) {
		xAxis = false;
		side = false;
		isPixel = true;
		elem = element;
		raw = pos;
		return parent;
	}
	IElement* IConstraint::Bottom(IElement* element, float pos) {
		xAxis = false;
		side = false;
		elem = element;
		raw = AlterSH(pos);
		return parent;
	}
	IElement* IConstraint::Top(int pos) {
		xAxis = true;
		side = true;
		isPixel = true;
		raw = pos;
		return parent;
	}
	IElement* IConstraint::Top(float pos) {
		xAxis = true;
		side = true;
		raw = AlterSH(pos);
		return parent;
	}
	IElement* IConstraint::Top(IElement* element, int pos) {
		xAxis = false;
		side = true;
		isPixel = true;
		elem = element;
		raw = pos;
		return parent;
	}
	IElement* IConstraint::Top(IElement* element, float pos) {
		xAxis = false;
		side = true;
		elem = element;
		raw = AlterSH(pos);
		return parent;
	}
	void IConstraint::Add(int pos) {
		raw += pos;
	}
	void IConstraint::Add(float pos) {
		if (xAxis) raw += AlterSW(pos);
		else raw += AlterSH(pos);
	}
	float IConstraint::Value(float f) {
		if (xAxis) {
			float temp = raw;
			float ex = 0;
			float ew = 0;
			if (isPixel) {
				temp = AlterW(raw);
			}
			if (elem != nullptr) {
				ex = elem->x;
				ew = elem->w;
			}
			if (center) {
				if (elem == nullptr) return temp;
				else return elem->x + temp;
			} else if (side) {
				if (elem == nullptr) return 1 - temp - f / 2;
				else return ex - (ew + f) / 2 - temp;
			} else {
				if (elem == nullptr) return temp - 1 + f / 2;
				else return ex + (ew + f) / 2 + temp;
			}
		} else {
			float temp = raw;
			float ey = 0;
			float eh = 0;
			if (isPixel) {
				temp = AlterH(raw);
			}
			if (elem != nullptr) {
				ey = elem->y;
				eh = elem->h;
			}
			if (center) {
				if (elem == nullptr) return temp;
				else return elem->y + temp;
			} else if (side) {
				if (elem == nullptr) return 1 - temp - f / 2;
				else return ey - (eh + f) / 2 - temp;
			} else {
				if (elem == nullptr) return temp - 1 + f / 2;
				else return ey + (eh + f) / 2 + temp;
			}
		}

		return 0;
	}
	ITransition::ITransition(bool* act) {
		activator = act;
	}
	ITransition::ITransition(std::function<bool()> act) {
		activatorF = act;
	}
	ITransition* ITransition::Fade(const Color& c, float time) {
		fade = true;
		color = c;
		fadeS = 1 / time;
		return this;
	}
	ITransition* ITransition::Move(int f0, int f1, float time) {
		move = true;
		movePixel = true;
		x = f0;
		y = f1;
		moveS = 1 / time;
		return this;
	}
	ITransition* ITransition::Move(float f0, float f1, float time) {
		move = true;
		x = f0;
		y = f1;
		moveS = 1 / time;
		return this;
	}
	ITransition* ITransition::Size(int f0, int f1, float time) {
		size = true;
		sizePixel = true;
		w = f0;
		h = f1;
		sizeS = 1 / time;
		return this;
	}
	ITransition* ITransition::Size(float f0, float f1, float time) {
		size = true;
		w = f0;
		h = f1;
		sizeS = 1 / time;
		return this;
	}
	float ITransition::ValueX() {
		if (movePixel) {
			return AlterW(x * moveT);
		} else {
			return x * moveT;
		}
	}
	float ITransition::ValueY() {
		if (movePixel) {
			return AlterH(y * moveT);
		} else {
			return y * moveT;
		}
	}
	float ITransition::ValueW() {
		if (sizePixel) {
			return AlterW(w * sizeT);
		} else {
			return w * sizeT;
		}
	}
	float ITransition::ValueH() {
		if (sizePixel) {
			return AlterH(h * sizeT);
		} else {
			return h * sizeT;
		}
	}
	void ITransition::Update(float delta) {
		/*if (func != nullptr) {
			if (*activator || funcT > 0)
				funcT += funcS * delta;
			if (funcT >= 1 && funcT < 1 + funcS * delta)
				func(funcData);
			if (funcT > 1 && !*activator)
				funcT = 0;
		}*/
		bool on = activator != nullptr ? *activator : activatorF();
		if (fade) {
			if (on) {
				fadeT += fadeS * delta;
			} else {
				fadeT -= fadeS * delta;
			}
			
			if (fadeT < 0) {
				fadeT = 0;
			} else if (fadeT > 1) {
				fadeT = 1;
			}
		}
		if (move) {
			if (on) {
				moveT += moveS * delta;
			} else {
				moveT -= moveS * delta;
			}
			if (moveT < 0) {
				moveT = 0;
			} else if (moveT > 1) {
				moveT = 1;
			}
		}
		if (size) {
			if (on) {
				sizeT += sizeS * delta;
			} else {
				sizeT -= sizeS * delta;
			}
			if (sizeT < 0) {
				sizeT = 0;
			} else if (sizeT > 1) {
				sizeT = 1;
			}
		}
	}
	IElement::IElement(const std::string& name, int priority) :
		name(name), conX(IConstraint(this, true)), conY(IConstraint(this, false)),
		conW(IConstraint(this, true)), conH(IConstraint(this, false)), priority(priority)
	{
		text.ElemWH(&w, &h);
	}
	IElement::~IElement() {

	}
	void IElement::Texture(const std::string& texture) {
		this->texture = texture;
	}
	void IElement::Col(const Color& c) {
		color = c;
	}
	void IElement::Blank() {
		isBlank = true;
	}

	void IElement::Text(Font* f, const std::string& s, const Color& color) {
		text.Setup(f, true);
		text.SetCol(color.r, color.g, color.b, color.a);
		text.SetText(s);
	}
	void IElement::Text(Font* f, const std::string& s, int charHeight, const Color& color) {
		text.Setup(f, true);
		text.SetCol(color.r, color.g, color.b, color.a);
		text.SetHeight(charHeight);
		text.SetText(s);
	}
	void IElement::Text(Font* f, const std::string& s, float charHeight, const Color& color) {
		text.Setup(f, true);
		text.SetCol(color.r, color.g, color.b, color.a);
		text.SetHeight(charHeight);
		text.SetText(s);
	}
	void IElement::SetText(const std::string& s) {
		text.SetText(s);
	}
	const std::string& IElement::GetText() {
		return text.text;
	}
	void IElement::SetEditable() {
		isEditable = true;
	}
	bool IElement::HasTags() {
		for (auto& b : tags) {
			if (!*b) return false;
		}
		for (auto& b : tagsF) {
			if (!b()) return false;
		}
		return true;
	}
	void IElement::AddTag(bool* f) {
		tags.push_back(f);
	}
	void IElement::AddTag(std::function<bool()> f) {
		tagsF.push_back(f);
	}
	ITransition* IElement::NewTransition(bool* activator) {
		if (activator != nullptr) {
			transitions.push_back(ITransition(activator));
			return &transitions.back();
		} else
			return nullptr;
	}
	ITransition* IElement::NewTransition(std::function<bool()> activator) {
		if (activator != nullptr) {
			transitions.push_back(ITransition(activator));
			return &transitions.back();
		} else
			return nullptr;
	}

	bool IElement::ClickEvent(int mx, int my, int button, int action) {
		if (action == 1)
			isSelected = false;
		//bug::outs < name < isSelected < bug::end;
		isHolding = false;
		if (Inside(mx, my)) {
			if (action == 1) {
				isHolding = true;
				isSelected = true;
				if (OnClick != nullptr) {
					OnClick(mx, my, button, action);
					return true;
				}
			}
		}
		//bug::outs < "" < isSelected < bug::end;
		return false;
	}
	bool IElement::ScrollEvent(double scroll) {
		if (isHovering) {
			if (OnScroll != nullptr) {
				OnScroll(scroll);
				return true;
			}
		}
		return false;
	}
	bool IElement::HoverEvent(int mx, int my) {
		if (Inside(mx, my)) {
			isHovering = true;
			if (OnHover != nullptr) {
				OnHover(mx, my);
				return true;
			} else {
				return false;
			}
		}
		isHovering = false;
		return false;
	}
	bool IElement::KeyEvent(int key, int action) {
		if (isSelected) {
			if (isEditable) {
				text.EditText(key, action);
			}
			if (OnKey != nullptr) {
				OnKey(key, action);
			}
			return isEditable || OnKey != nullptr;
		}
		return false;
	}
	/*
	void IElement::ResizeEvent(int width, int height) {

	}
	*/
	void IElement::InternalUpdate(float delta) {
		for (ITransition& t : transitions) {
			t.Update(delta);
		}
	}
	void IElement::Update(float delta) {
		
	}
	void IElement::Draw() {
		
		// Move constrain calculations somewhere?
		w = conW.Value(0);
		h = conH.Value(0);
		x = conX.Value(w);
		y = conY.Value(h);

		isBlank = false;
		// Transitions
		if (isBlank) {
			for (auto& t : transitions) {
				if (t.move) {
					x += t.ValueX();
					y += t.ValueY();
				}
				if (t.size) {
					w += t.ValueW();
					h += t.ValueH();
				}
			}
		} else {
			float red = 0;
			float green = 0;
			float blue = 0;
			float alpha = 0;
			float redBase = color.r;
			float greenBase = color.g;
			float blueBase = color.b;
			float alphaBase = color.a;
			int fadeCount = 0;
			for (auto& t : transitions) {
				if (t.fade) {
					redBase *= (1 - t.fadeT);
					greenBase *= (1 - t.fadeT);
					blueBase *= (1 - t.fadeT);
					alphaBase *= (1 - t.fadeT);
					fadeCount++;
				}
			}
			red += redBase;
			green += greenBase;
			blue += blueBase;
			alpha += alphaBase;
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
					red += redBase;
					green += greenBase;
					blue += blueBase;
					alpha += alphaBase;
				}
				if (t.move) {
					x += t.ValueX();
					y += t.ValueY();
				}
				if (t.size) {
					w += t.ValueW();
					h += t.ValueH();
				}
			}

			// Rendering
			BindTexture(0,texture);
			DrawRect(x, y, w, h, red, green, blue, alpha);

			// Text
			if (text.text.length() > 0 || isEditable && isSelected) {
				text.SetPos(x, y);
				GuiSize(1, 1);
				text.DrawString(alpha, isEditable && isSelected);
			}
		}
	}
	bool IElement::Inside(float mx, float my) {
		mx = AlterX(mx);
		my = AlterY(my);
		//bug::outs < mx<my< x < y < w < h < bug::end;
		//bug::outs < (mx > x - w / 2) < (mx<x + w / 2) < (my>w - h / 2) < (my < y + h / 2) < bug::end;
		return mx > x - w / 2 && mx<x + w / 2 && my>y - h / 2 && my < y + h / 2;
	}
}