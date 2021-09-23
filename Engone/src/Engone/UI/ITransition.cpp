#include "gonpch.h"

#include "ITransition.h"

#include "../Rendering/Renderer.h"

namespace engone {
	ITransition::ITransition(bool* act) {
		activator = act;
	}
	ITransition::ITransition(std::function<bool()> act) {
		activatorF = act;
	}
	ITransition* ITransition::Fade(const IColor& c, float time) {
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
			return renderer->ToFloatScreenW(x * moveT);
		}
		else {
			return x * moveT;
		}
	}
	float ITransition::ValueY() {
		if (movePixel) {
			return renderer->ToFloatScreenH(y * moveT);
		}
		else {
			return y * moveT;
		}
	}
	float ITransition::ValueW() {
		if (sizePixel) {
			return renderer->ToFloatScreenW(w * sizeT);
		}
		else {
			return w * sizeT;
		}
	}
	float ITransition::ValueH() {
		if (sizePixel) {
			return renderer->ToFloatScreenH(h * sizeT);
		}
		else {
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
			if (on) {
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
			if (on) {
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
}