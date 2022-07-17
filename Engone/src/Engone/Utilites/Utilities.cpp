
#include "Engone/Utilities/Utilities.h"

#include <GLFW/glfw3.h>

namespace engone {

	std::vector<std::string> SplitString(std::string text, std::string delim) {
		std::vector<std::string> out;
		unsigned int at = 0;
		while ((at = text.find(delim)) != std::string::npos) {
			std::string push = text.substr(0, at);
			if (push.size() > 0) {
				out.push_back(push);
			}
			text.erase(0, at + delim.length());
		}
		if(text.size()!=0)
			out.push_back(text);
		return out;
	}
	char SanitizedChars[64]{
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'0','1','2','3','4','5','6','7','8','9','_','-'
	};
	std::string SanitizeString(std::string s) {
		std::string out;
		for (uint32_t i = 0; i < s.size(); ++i) {
			for (uint32_t j = 0; j < 64; ++j) {
				if (s.at(i) == SanitizedChars[j]) {
					out += s.at(i);
					break;
				}
			}
		}
		return out;
	}
	/*
	void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3) {
		ar[ind] = f0;
		ar[ind + 1] = f1;
		ar[ind + 2] = f2;
		ar[ind + 3] = f3;
	}*/
	float lerp(float a, float b, float c) {
		return (1 - c) * a + c * b;
	}
	float inverseLerp(float min, float max, float x) {
		return (x - min) / (max - min);
	}
	float distance(float x, float y, float x1, float y1) {
		return (float)sqrt(pow(x1 - x, 2) + pow(y1 - y, 2));
	}
	/*
	void insert(float* ar, int off, int len, float* data) { // carefull with going overboard
		//std::cout << "INSERT ";
		for (int i = 0; i < len; i++) {
			ar[off + i] = data[i];
			//std::cout << off + i << "_" << ar[off + i] << "   ";
		}
		//std::cout << std::endl;
	}*/
	float bezier(float x, float xStart, float xEnd) {
		float t = (x - xStart) / (xEnd - xStart);
		float va = /*(pow(1 - t, 3) + 3 * pow(1 - t, 2) * t) +*/(3 * (1 - t) * (float)pow(t, 2) + (float)pow(t, 3));
		return va;
	}
	/*
	std::string Crypt(const std::string& word, const std::string& key, bool encrypt) {
		std::string out = "";
		for (int i = 0; i < word.length();i++) {
			int at = word[i];
			for (int j = 0; j < key.length();j++) {
				int val = (i - j) * key[j];
				if (encrypt) at += val;
				else at -= val;
			}
			out += (at % 256);
		}
		return out;
	}*/

	// double in seconds, microsecond precision
	double GetSystemTime() {
		return (double)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()/1000000.0;
		//std::cout << std::chrono::system_clock::now().time_since_epoch().count() <<" "<< (std::chrono::system_clock::now().time_since_epoch().count() / 10000000) << "\n";
		//return (double)(std::chrono::system_clock::now().time_since_epoch().count() / 10000000);
	}
	// how do this
	static std::mt19937 utility_mt19937_random;
	static std::uniform_real_distribution utility_uniform_distrubtion;
	static bool once = false;
	float GetRandom() {
		if (!once) {
			utility_mt19937_random.seed((uint32_t)GetSystemTime());
			once = true;
		}
		return (float)utility_uniform_distrubtion(utility_mt19937_random);
	}
	std::string GetClock() {
		time_t t;
		time(&t);
		tm tm;
		localtime_s(&tm, &t);
		std::string str = std::to_string(tm.tm_hour) + ":" + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec);
		return str;
	}
	std::string FormatTime(double seconds, bool compact, FormatTimePeriods flags) {
		bool small = false;
		for (int i = 0; i < 3;i++) {
			if (flags & (1 << i))
				small = true;
			if (small) seconds *= 1000;
		}
		return FormatTime((uint64_t)round(seconds),compact,flags);
	}
	std::string FormatTime(uint64_t time, bool compact, FormatTimePeriods flags) {
		// what is the max amount of characters?
		const int outSize = 130;
		char out[outSize]{};
		int write = 0;

		uint64_t num[8]{};
		uint64_t divs[8]{ 1,1,1,1,1,1,1,1 };
		uint64_t divLeaps[8]{ 1000,1000,1000,60,60,24,7,1 };
		const char* lit[8]{"nanosecond","microsecond","millisecond","second","minute","hour","day","week"};

		uint64_t rest = time;

		bool smallest = false;
		for (int i = 0; i < 7; i++) {
			if (flags & (1 << i))
				smallest = true;
			if (smallest)
				divs[i + 1] *= divs[i]*divLeaps[i];
		}

		for (int i = 7; i >= 0;i--) {
			if (0==(flags & (1 << i))) continue;
			num[i] = rest/divs[i];
			if (num[i] == 0) continue;
			rest -= num[i] * divs[i];

			if (write != 0) out[write++] = ' ';
			write += snprintf(out + write, outSize - write, "%u ", num[i]);
			if (compact) {
				out[write++] = lit[i][0];
				if (i > 4)
					out[write++] = 's';
			} else {
				memcpy(out + write, lit[i], strlen(lit[i]));
				write += strlen(lit[i]);
				if(num[i]!=1)
					out[write++] = 's';
			}
		}
		return out;
	}
	/*
	void CountingTest(int times, int numElements, std::function<int()> func) {
		std::vector<int> occurrences(numElements);

		double bef = GetSystemTime();
		for (int i = 0; i < times; i++) {
			int index = func();
			if(index>-1&&index<numElements)
				occurrences[func()]++;
		}
		double aft = GetSystemTime();
		for (int i = 0; i < numElements; i++) {
			std::cout << i << " " << occurrences[i] << std::endl;
		}
		std::cout << "Time: " << (aft - bef) << std::endl;
	}*/

	//static std::unordered_map<int, int> timerCounting;
	//Timer::Timer() : time(GetAppTime()) { }
	//Timer::Timer(const std::string& str) : time(GetAppTime()), name(str) { }
	//Timer::Timer(const std::string& str, int id) : time(GetAppTime()), name(str), id(id) { }
	//Timer::~Timer() {
	//	if (time != 0) end();
	//}
	//void Timer::end() {
	//	if (id != 0) {
	//		if (timerCounting.count(id)>0) {
	//			timerCounting[id]++;
	//			if (timerCounting[id] < 60) {
	//				return;
	//			}else{
	//				timerCounting[id] = 0;
	//			}
	//		} else {
	//			timerCounting[id] = 0;
	//			return;
	//		}
	//	}

	//	log::out << name << " : " << (GetAppTime() - time) << "\n";
	//	time = 0;
	//}
	bool FindFile(const std::string& path) {
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}
	std::vector<std::string> GetFiles(const std::string& path) {
		std::vector<std::string> list;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			list.push_back(entry.path().generic_string());
		}
		return list;
	}
	// In seconds
	uint64_t GetFileLastModified(const std::string& path) {
		auto some = std::filesystem::last_write_time(path);
		if (std::filesystem::exists(path))
			return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(path).time_since_epoch()).count();
		//log::out << log::RED << "getTime - path not valid\n";
		return 0;
	}


	// memory tracker
	static uint32_t allocatedMemory = 0;
	void* _malloc(uint32_t size) {
		if (size == 0) return nullptr;
		void* ptr = malloc(size);
		if (ptr) allocatedMemory += size;
		return ptr;
	}
	void* _realloc(void* ptr, uint32_t oldSize, uint32_t newSize) {
		if (newSize == 0) {
			_free(ptr, oldSize);
			return nullptr;
		}
		void* newPtr = realloc(ptr, newSize);
		if (newPtr) {
			allocatedMemory += newSize - oldSize;
			return newPtr;
		} else {
			return ptr;
		}
	}
	void _free(void* ptr, uint32_t size) {
		if (!ptr) return;
		allocatedMemory -= size;
		free(ptr);
	}
	void _logMemory() {
		std::cout << "Memory: " << allocatedMemory << "\n";
	}
	uint32_t _getMemory() {
		return allocatedMemory;
	}

	// FILE
	const char* toString(FileError e) {
		switch (e) {
		case FileErrorMissing: return "Missing File";
		case FileErrorCorrupted: return "Corrupted Data";
		}
		return "";
	}
	namespace log {
		logger operator<<(logger log, FileError err) {
			return log << toString(err);
		}
	}
}