#include "IBase.h"
#include "../Rendering/Renderer.h"

namespace engine {

	IConstraintX::IConstraintX(IBase* b) : _this(b) {}
	IBase* IConstraintX::Center(int pos) {
		center = true;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintX::Center(float pos) {
		center = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintX::Center(IBase* b, int pos) {
		center = true;
		parent = b;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintX::Center(IBase* b, float pos) {
		center = true;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraintX::Left(int pos) {
		sticky = false;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintX::Left(float pos) {
		sticky = false;
		raw = pos;
		return _this;
	}
	IBase* IConstraintX::Left(IBase* b, int pos) {
		sticky = false;
		isPixel = true;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraintX::Left(IBase* b, float pos) {
		sticky = false;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraintX::Right(int pos) {
		sticky = true;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintX::Right(float pos) {
		sticky = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintX::Right(IBase* b, int pos) {
		sticky = true;
		isPixel = true;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraintX::Right(IBase* b, float pos) {
		sticky = true;
		parent = b;
		raw = pos;
		return _this;
	}
	void IConstraintX::Add(int pos) {
		raw += pos;
	}
	void IConstraintX::Add(float pos) {
		raw += pos;
	}
	float IConstraintX::Value() {
		float move = raw;
		float px = 0;
		float pw = 0;
		if (isPixel) {
			move = ToFloatScreenW(raw);
		}
		
		if (parent != nullptr) {
			px = parent->x;
			pw = parent->w;
		}
		if (center) {
			if (parent == nullptr) return move - _this->w/2;
			else return px + move - _this->w/2;
		}
		else if (sticky) {
			if (parent == nullptr) return 1 - move - _this->w;
			else return px - move - _this->w;
		}
		else {
			if (parent == nullptr) return move - 1;
			else return px + pw + move;
		}
	}

	IConstraintY::IConstraintY(IBase* b) : _this(b) {}
	IBase* IConstraintY::Center(int pos) {
		center = true;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintY::Center(float pos) {
		center = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintY::Center(IBase* b, int pos) {
		center = true;
		parent = b;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintY::Center(IBase* b, float pos) {
		center = true;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraintY::Bottom(int pos) {
		sticky = false;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintY::Bottom(float pos) {
		sticky = false;
		raw = pos;
		return _this;
	}
	IBase* IConstraintY::Bottom(IBase* b, int pos) {
		sticky = false;
		isPixel = true;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraintY::Bottom(IBase* b, float pos) {
		sticky = false;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraintY::Top(int pos) {
		sticky = true;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintY::Top(float pos) {
		sticky = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintY::Top(IBase* b, int pos) {
		sticky = true;
		isPixel = true;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraintY::Top(IBase* b, float pos) {
		sticky = true;
		parent = b;
		raw = pos;
		return _this;
	}
	void IConstraintY::Add(int pos) {
		raw += pos;
	}
	void IConstraintY::Add(float pos) {
		raw += pos;
	}
	float IConstraintY::Value() {
		float move = raw;
		float py = 0;
		float ph = 0;
		if (isPixel) {
			move = ToFloatScreenH(raw);
		}
		
		if (parent != nullptr) {
			py = parent->y;
			ph = parent->h;
		}
		if (center) {
			if (parent == nullptr) return move - _this->h/2;
			else return py + move - _this->h/2;
		}
		else if (sticky) {
			if (parent == nullptr) return 1 - move - _this->h;
			else return py - move - _this->h;
		}
		else {
			if (parent == nullptr) return move - 1;
			else return py + move;
		}
	}
	IConstraintW::IConstraintW(IBase* b) : _this(b) {}
	IBase* IConstraintW::Center(int pos) {
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintW::Center(float pos) {
		raw = pos;
		return _this;
	}
	IBase* IConstraintW::Center(IBase* b, int pos) {
		parent = b;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintW::Center(IBase* b, float pos) {
		parent = b;
		raw = pos;
		return _this;
	}
	void IConstraintW::Add(int pos) {
		raw += pos;
	}
	void IConstraintW::Add(float pos) {
		raw += pos;
	}
	float IConstraintW::Value() {
		float move = raw;
		float pw = 0;
		if (isPixel) {
			move = ToFloatScreenW(raw);
		}
		
		if (parent != nullptr) {
			pw = parent->w;
		}
		if (parent == nullptr) return move;
		else return pw + move;
	}
	IConstraintH::IConstraintH(IBase* b) : _this(b) {}
	IBase* IConstraintH::Center(int pos) {
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintH::Center(float pos) {
		raw = pos;
		return _this;
	}
	IBase* IConstraintH::Center(IBase* b, int pos) {
		parent = b;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraintH::Center(IBase* b, float pos) {
		parent = b;
		raw = pos;
		return _this;
	}
	void IConstraintH::Add(int pos) {
		raw += pos;
	}
	void IConstraintH::Add(float pos) {
		raw += pos;
	}
	float IConstraintH::Value() {
		float move = raw;
		float ph = 0;
		if (isPixel) {
			move = ToFloatScreenH(raw);
		}
		
		if (parent != nullptr) {
			ph = parent->h;
		}
		if (parent == nullptr) return move;
		else return ph + move;
	}
}