#include "gonpch.h"

#include "IColor.h"

namespace engine {
	IColor::IColor()
		: r(1), g(1), b(1), a(1) {}
	IColor::IColor(float f)
		: r(f), g(f), b(f), a(f) { }
	IColor::IColor(int i)
		: r(i / 256.f), g(i / 256.f), b(i / 256.f), a(i/256.f) { }
	IColor::IColor(float f, float a)
		: r(f), g(f), b(f), a(a) { }
	IColor::IColor(int i, int a)
		: r(i / 256.f), g(i / 256.f), b(i / 256.f), a(a/256.f) { }
	IColor::IColor(float red, float green, float blue)
		: r(red), g(green), b(blue), a(1) { }
	IColor::IColor(int red, int green, int blue)
		: r(red / 256.f), g(green / 256.f), b(blue / 256.f), a(1) { }
	IColor::IColor(float red, float green, float blue, float alpha)
		: r(red), g(green), b(blue), a(alpha) { }
	IColor::IColor(int red, int green, int blue, int alpha)
		: r(red / 256.f), g(green / 256.f), b(blue / 256.f), a(alpha/256.f) { }
}