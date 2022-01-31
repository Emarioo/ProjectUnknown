#pragma once

namespace engone {
	std::vector<std::string> SplitString(std::string text, std::string delim);
	std::string SanitizeString(std::string s);
	float lerp(float a, float b, float c);
	float inverseLerp(float min, float max, float x);
	float distance(float x, float y, float x1, float y1);
	//void insert(float* ar, int off, int len, float* data);
	float bezier(float x, float xStart, float xEnd);
	//std::string Crypt(const std::string& word,const std::string& key, bool encrypt);

	//void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3);
	// The time the program(glfw) has been running in seconds
	double GetAppTime();
	//The time since epoch in seconds
	double GetSystemTime();
	std::string GetClock();
	double GetRandom();

	//void CountingTest(int times, int numElements, std::function<int()> func);

	template<typename Base, typename T>
	inline bool instanceof(const T*) {
		return std::is_base_of<Base, T>::value;
	}

	template<typename Base, typename T>
	inline Base instance(const T*) {
		return std::is_base_of<Base, T>;
	}
}