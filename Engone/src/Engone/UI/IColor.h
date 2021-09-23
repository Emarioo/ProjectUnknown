#pragma once

namespace engone {
	class IColor {
	public:
		float r, g, b, a;
		IColor();
		IColor(float rgba);
		IColor(int rgba);
		IColor(float rgb, float alpha);
		IColor(int rgb, int alpha);
		IColor(float red, float green, float blue);
		IColor(int red, int green, int blue);
		IColor(float red, float green, float blue, float alpha);
		IColor(int red, int green, int blue, int alpha);
	};
}