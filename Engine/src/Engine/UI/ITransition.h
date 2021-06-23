#pragma once

#include <functional>

#include "IColor.h"

namespace engine {
	class ITransition {
	private:
		// std::function<void(int)> func = nullptr;
		// float funcS = 0, funcT = 0;
		float fadeS = 0, moveS = 0, sizeS = 0;
		bool* activator = nullptr;
		std::function<bool()> activatorF = nullptr;
		bool movePixel = false, sizePixel = false;
		float x = 0, y = 0, w = 0, h = 0;
	public:
		bool fade = false, move = false, size = false;
		//float r = 1, g = 1, b = 1, a = 1;
		IColor color;
		float fadeT = 0, moveT = 0, sizeT = 0;

		ITransition(bool* act);
		ITransition(std::function<bool()> act);

		ITransition* Fade(const IColor& color, float time);
		ITransition* Move(int x, int y, float time);
		ITransition* Move(float x, float y, float time);
		ITransition* Size(int w, int h, float time);
		ITransition* Size(float w, float h, float time);

		float ValueX();
		float ValueY();
		float ValueW();
		float ValueH();

		// Fade is calculated in element.cpp since it requires some more hardcore calculations
		// Might be possible to make a functions out of it but ehh, whatever

		void Update(float delta);
	};
}