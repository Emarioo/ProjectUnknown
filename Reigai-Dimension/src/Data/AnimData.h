#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>

class Keyframe {
public:
	Keyframe(){}
	Keyframe(char, unsigned short, float);
	char polation;
	unsigned short frame;
	float value;
};
class FCurve {
public:
	FCurve(){}
	
	float GetValue(int frame);
	std::vector<Keyframe> keyframes;
};
class FCurves {
public:
	FCurves(){}
	unsigned short fCurves;
	bool CurveExist(int n);
	/*
	bool curves[9]{ 0,1,1,0,0,1,0,0,1 };
	short curveInt = 0;
	for (int i = 0; i < 9;i++) {
		curveInt += curves[i]*pow(2,i);
	}
	std::cout << curveInt << std::endl;

	for (int i = 8; i >= 0; i--) {
		if (0 <= curveInt - pow(2, i)) {
			curveInt -= pow(2, i);
			curves[i] = 1;
		} else {
			curves[i] = 0;
		}
		std::cout << curves[i];
	}
	std::cout << std::endl;
	*/
	//std::map<int, FCurve> keyframes;
	FCurve keyframes[9];
};
class AnimData {
public:
	AnimData(){}
	std::map<std::string, FCurves> curves;
	unsigned short frameStart=0;
	unsigned short frameEnd=0;
	float defaultSpeed=1;
	bool loop=0;

	bool hasError = false;
};
