#pragma once

#include "Rendering/Renderer.h"

class IPos { // TODO: pixelScreen is out of function
	bool pixelScreen = false;
	float x = 0, y = 0;
public:
	float GetX() {
		if (pixelScreen) {
			return AlterX(x);
		}
		return x;
	}
	float GetY() {
		if (pixelScreen) {
			return AlterY(y);
		}
		return y;
	}
	IPos() {
	}
	void Data(float a, float b) {
		x = a;
		y = b;
	}
	void Data(bool f, float a, float b) {// pixels
		if (f) {
			x = a;
			y = b;
			pixelScreen = true;
		} else {
			x = AlterW(a);
			y = AlterH(b);
		}
	}
	void DataY(float a) {
		y = a;
	}
	void DataY(bool f, float a) {// pixels
		if (f) {
			y = a;
			pixelScreen = true;
		} else {
			y = AlterH(a);
		}
	}
};
class ISize {
public:
	bool pixelScreen = false;
	float w = 0, h = 0;
	float GetW() {
		if (pixelScreen) {
			return AlterW(w);
		}
		return w;
	}
	float GetH() {
		if (pixelScreen) {
			return AlterH(h);
		}
		return h;
	}
	ISize() {}
	void Data(float a, float b) {
		w = a;
		h = b;
	}
	void Data(bool f, float a, float b) {
		if (f) {
			w = a;
			h = b;
			pixelScreen = true;
		} else {
			w = AlterW(a);
			h = AlterH(b);
		}
	}
};