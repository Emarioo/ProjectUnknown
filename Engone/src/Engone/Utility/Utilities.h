#pragma once

#include "../Logger.h"

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

	// A vector but you can have differently sized classes and structs.
	class ItemVector {
	private:
		char* data = nullptr;
		int maxSize = 0;
		int writeIndex = 0;
		int readIndex = 0;

	public:
		ItemVector(int size = 0) {
			if (size != 0) {
				data = (char*)malloc(size);
				if (data)
					maxSize = size;
				else
					log::out << log::RED << "failed allocating memory\n";
			}
		}

		template<class T>
		void writeMemory(char type, void* ptr) {
			int itemSize = sizeof(T);
			if (maxSize < writeIndex + sizeof(char) + itemSize) {
				if (maxSize == 0)
					maxSize = 5;

				if(maxSize*2<writeIndex+sizeof(char)+itemSize) {
					maxSize += (sizeof(char) + itemSize)*2;
				} else {
					maxSize *= 2;
				}

				char* newPtr = (char*)realloc(data, maxSize);
				if (newPtr) {
					data = newPtr;
				} else {
					maxSize = 0;
					log::out << log::RED << "failed reallocating memory\n";
					return;
				}
			}
			*(data + writeIndex) = type;
			std::memcpy(data + writeIndex + sizeof(char), ptr, itemSize);
			writeIndex += sizeof(char) + itemSize;
		}
		char readType() {
			if (writeIndex < readIndex + sizeof(char)) {
				return 0;
			}

			char type = *(data + readIndex);
			readIndex += sizeof(char);
			return type;
		}
		template<class T>
		T* readItem() {
			if (writeIndex < readIndex + sizeof(T)) {
				log::out << "reached end of HeapMemory\n";
				return nullptr;
			}

			char* ptr = data + readIndex;
			readIndex += sizeof(T);
			return (T*)ptr;
		}
		void clear() {
			writeIndex = 0;
			readIndex = 0;
		}
	};

	class Timer {
	private:
		double time;
		std::string name;
		int id=0;
	public:
		Timer();
		Timer(const std::string& str);
		Timer(const std::string& str, int id);
		~Timer();
		void end();
	};

	#define TIMER(str) Timer str = Timer(#str,__LINE__*strlen(__FILE__))
	
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