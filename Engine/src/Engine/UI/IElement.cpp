#include "IElement.h"

namespace engine {

	IElement::IElement(const std::string& name, int priority) :
		IBase(name), priority(priority)
	{
		text.ElemWH(&w, &h);
	}
	IElement::~IElement() {

	}
	void IElement::Texture(const std::string& texture) {
		this->texture = texture;
	}
	void IElement::Blank() {
		isBlank = true;
	}
	void IElement::Text(Font* f, const std::string& s, const IColor& color) {
		text.Setup(f, true);
		text.SetCol(color.r, color.g, color.b, color.a);
		text.SetText(s);
	}
	void IElement::Text(Font* f, const std::string& s, int charHeight, const IColor& color) {
		text.Setup(f, true);
		text.SetCol(color.r, color.g, color.b, color.a);
		text.SetHeight(charHeight);
		text.SetText(s);
	}
	void IElement::Text(Font* f, const std::string& s, float charHeight, const IColor& color) {
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
	void IElement::Render() {
		
		// Move constrain calculations somewhere?

		CalcConstraints();

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
				text.SetPos(x+w/2, y+h/2);
				GuiSize(1, 1);
				text.DrawString(alpha, isEditable && isSelected);
			}
		}
	}
	bool IElement::Inside(float mx, float my) {
		mx = ToFloatScreenX(mx);
		my = ToFloatScreenY(my);
		//bug::outs < mx<my< x < y < w < h < bug::end;
		//bug::outs < (mx > x - w / 2) < (mx<x + w / 2) < (my>w - h / 2) < (my < y + h / 2) < bug::end;
		return mx > x && mx<x + w && my>y && my < y + h;
	}
}