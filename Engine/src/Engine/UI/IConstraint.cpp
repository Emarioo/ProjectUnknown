#include "IBase.h"
#include "Rendering/Renderer.h"

namespace engine {

	IConstraint::IConstraint(IBase* b, bool use_xAxis) : _this(b), xAxis(use_xAxis) {}
	IBase* IConstraint::Center(int pos) {
		
		center = true;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraint::Center(float pos) {
		center = true;
		if (xAxis) raw = PercentToFloatScreenW(pos);
		else raw = PercentToFloatScreenH(pos);
		return _this;
	}
	IBase* IConstraint::Center(IBase* b, int pos) {
		center = true;
		parent = b;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraint::Center(IBase* b, float pos) {
		center = true;
		parent = b;
		if (xAxis) raw = PercentToFloatScreenW(pos);
		else raw = PercentToFloatScreenH(pos);
		return _this;
	}
	IBase* IConstraint::Left(int pos) {
		xAxis = true;
		side = false;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraint::Left(float pos) {
		xAxis = true;
		side = false;
		raw = PercentToFloatScreenW(pos);
		return _this;
	}
	IBase* IConstraint::Left(IBase* b, int pos) {
		xAxis = true;
		side = false;
		isPixel = true;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraint::Left(IBase* b, float pos) {
		xAxis = true;
		side = false;
		parent = b;
		raw = PercentToFloatScreenW(pos);
		return _this;
	}
	IBase* IConstraint::Right(int pos) {
		xAxis = true;
		side = true;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraint::Right(float pos) {
		xAxis = true;
		side = true;
		raw = PercentToFloatScreenW(pos);
		return _this;
	}
	IBase* IConstraint::Right(IBase* b, int pos) {
		xAxis = true;
		side = true;
		isPixel = true;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraint::Right(IBase* b, float pos) {
		xAxis = true;
		side = true;
		parent = b;
		raw = PercentToFloatScreenW(pos);
		return _this;
	}
	IBase* IConstraint::Bottom(int pos) {
		xAxis = false;
		side = false;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraint::Bottom(float pos) {
		xAxis = false;
		side = false;
		raw = PercentToFloatScreenH(pos);
		return _this;
	}
	IBase* IConstraint::Bottom(IBase* b, int pos) {
		xAxis = false;
		side = false;
		isPixel = true;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraint::Bottom(IBase* b, float pos) {
		xAxis = false;
		side = false;
		parent = b;
		raw = PercentToFloatScreenH(pos);
		return _this;
	}
	IBase* IConstraint::Top(int pos) {
		xAxis = true;
		side = true;
		isPixel = true;
		raw = pos;
		return _this;
	}
	IBase* IConstraint::Top(float pos) {
		xAxis = true;
		side = true;
		raw = PercentToFloatScreenH(pos);
		return _this;
	}
	IBase* IConstraint::Top(IBase* b, int pos) {
		xAxis = false;
		side = true;
		isPixel = true;
		parent = b;
		raw = pos;
		return _this;
	}
	IBase* IConstraint::Top(IBase* b, float pos) {
		xAxis = false;
		side = true;
		parent = b;
		raw = PercentToFloatScreenH(pos);
		return _this;
	}
	void IConstraint::Add(int pos) {
		raw += pos;
	}
	void IConstraint::Add(float pos) {
		if (xAxis) raw += PercentToFloatScreenW(pos);
		else raw += PercentToFloatScreenH(pos);
	}
	float IConstraint::Value(float f) {
		if (xAxis) {
			float temp = raw;
			float ex = 0;
			float ew = 0;
			if (isPixel) {
				temp = ToFloatScreenW(raw);
			}
			if (parent != nullptr) {
				ex = parent->x;
				ew = parent->w;
			}
			if (center) {
				if (parent == nullptr) return temp;
				else return parent->x + temp;
			}
			else if (side) {
				if (parent == nullptr) return 1 - temp - f / 2;
				else return ex - (ew + f) / 2 - temp;
			}
			else {
				if (parent == nullptr) return temp - 1 + f / 2;
				else return ex + (ew + f) / 2 + temp;
			}
		}
		else {
			float temp = raw;
			float ey = 0;
			float eh = 0;
			if (isPixel) {
				temp = ToFloatScreenH(raw);
			}
			if (parent != nullptr) {
				ey = parent->y;
				eh = parent->h;
			}
			if (center) {
				if (parent == nullptr) return temp;
				else return parent->y + temp;
			}
			else if (side) {
				if (parent == nullptr) return 1 - temp - f / 2;
				else return ey - (eh + f) / 2 - temp;
			}
			else {
				if (parent == nullptr) return temp - 1 + f / 2;
				else return ey + (eh + f) / 2 + temp;
			}
		}
		return 0;
	}
}