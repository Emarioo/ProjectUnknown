#include "AnimData.h"

Keyframe::Keyframe(char p, unsigned short f, float v)
	: polation(p), frame(f), value(v) {

}
bool FCurves::CurveExist(int n) {
	unsigned short curveInt = fCurves;
	for (int i = 8; i >= 0; i--) {
		if (0 <= curveInt - pow(2, i)) {
			curveInt -= pow(2, i);
			if (i == n)
				return 1;
		} else {
			if (i == n)
				return 0;
		}
	}
}
float bezier(float x, float xStart, float xEnd, float y0, float y1) {
	float t = (x - xStart) / (xEnd - xStart);
	float va = (pow(1 - t, 3) + 3 * pow(1 - t, 2) * t) * y0 + (3 * (1 - t) * pow(t, 2) + pow(t, 3)) * y1;
	return va;
}
float FCurve::GetValue(int frame) {
	float v[2]{ 0,0 };
	int f[2]{ 0,0 };
	char p = ' ';
	for (Keyframe k : keyframes) {
		if (k.frame <= frame) {
			f[0] = k.frame;
			v[0] = k.value;
			p = k.polation;
		}
		if (k.frame >= frame) {
			f[1] = k.frame;
			v[1] = k.value;
			break;
		}
	}
	if (v[0] == v[1]) {
		return v[0];
	}

	float out = 0;
	if (p == 'C') {
		out = v[0];
	} else if (p == 'L') {
		float t = ((float)frame - f[0]) / (f[1] - f[0]);
		out = (1 - t) * v[0] + t * v[1];
	} else if (p == 'B') {
		out = bezier(frame, f[0], f[1], v[0], v[1]);
	}
	return out;
}