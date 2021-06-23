#pragma once

namespace engine {
	class IColor {
	public:
		float r, g, b, a;
		IColor();
		IColor(float gray);
		IColor(int gray);
		IColor(float gray, float alpha);
		IColor(int gray, float alpha);
		IColor(float red, float green, float blue);
		IColor(int red, int green, int blue);
		IColor(float red, float green, float blue, float alpha);
		IColor(int red, int green, int blue, float alpha);
	};
}