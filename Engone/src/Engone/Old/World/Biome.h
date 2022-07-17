#pragma once

namespace engone {

	class Biome {
	private:
		/*
		Height lerp
		*/
		float y0 = 0, y1 = 1;
		/*
		Color lerp
		*/
		float c0 = 1, c1 = 1, c2 = 1, c3 = 1, c4 = 1, c5 = 1;
		/*
		Flat between f0 f1
		Height f2
		Multiplier f3
		*/
		float f0 = 0, f1 = 0, f2 = 0, f3 = 1;
		/*
		Smoothing
		*/
		float s0 = 0, s1 = 0;
	public:
		/*
		Rarity
		*/
		float r;
		Biome() {}
		Biome(float f0, float f1, float f2) {
			Rarity(f0);
			Height(f1, f2);
		}
		void Rarity(float f0) {
			r = f0;
		}
		void Height(float f0, float f1) {
			y0 = f0;
			y1 = f1;
		}
		void Color(float f0, float f1, float f2, float f3, float f4, float f5) {
			c0 = f0 / 256;
			c1 = f1 / 256;
			c2 = f2 / 256;
			c3 = f3 / 256;
			c4 = f4 / 256;
			c5 = f5 / 256;
		}
		void ColorD(float f0, float f1, float f2, float f3, float f4, float f5) {
			c0 = (f0 - f1) / 256;
			c1 = (f0 + f1) / 256;
			c2 = (f2 - f3) / 256;
			c3 = (f2 + f3) / 256;
			c4 = (f4 - f5) / 256;
			c5 = (f4 + f5) / 256;
		}
		void Color(float f0, float f1, float f2) {
			c0 = f0 / 256;
			c1 = f0 / 256;
			c2 = f1 / 256;
			c3 = f1 / 256;
			c4 = f2 / 256;
			c5 = f2 / 256;
		}
		/*
		f0, f1 range of a
		a = f2 + a*f3
		*/
		void Flat(float f0, float f1, float f2, float f3) {
			this->f0 = f0;
			this->f1 = f1;
			this->f2 = f2;
			this->f3 = f3;
		}
		void Smooth(float f0, float f1) {
			s0 = f0;
			s1 = f1;
		}
		/*
		value from noise map
		v is a float[6]{x,y,z,r,g,b}
		*/
		bool Biomify(float r0, float value, float* v) { // TODO: Improve performance
			if (value < r) {
				float a = (value - r0) / (r - r0);

				if (s0 != 0)
					a = (int)(a / s0) * s0 + a * s1;
				
				if (a >= f0 && a < f1)
					a = f2 + a * f3;
				v[1] = y0 + a * (y1 - y0);

				v[3] = c0 + a * (c1 - c0);
				v[4] = c2 + a * (c3 - c2);
				v[5] = c4 + a * (c5 - c4);


				return true;
			}
			return false;
		}
	};
}