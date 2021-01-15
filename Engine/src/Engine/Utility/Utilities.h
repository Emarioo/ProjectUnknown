#pragma once

#include <vector>
#include <string>

namespace engine {
	std::vector<std::string> SplitString(std::string text, std::string delim);
	std::string SanitizeString(std::string s);
	float lerp(float a, float b, float c);
	float inverseLerp(float min, float max, float x);
	float distance(float x, float y, float x1, float y1);
	void insert(float* ar, int off, int len, float* data);
	float bezier(float x, float xStart, float xEnd);

	void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3);

	template<typename Base, typename T>
	inline bool instanceof(const T*) {
		return std::is_base_of<Base, T>::value;
	}

	template<typename Base, typename T>
	inline Base instance(const T*) {
		return std::is_base_of<Base, T>;
	}
}