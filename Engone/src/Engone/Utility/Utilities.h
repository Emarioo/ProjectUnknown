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
	float GetAppTime();
	//The time since epoch in seconds
	double GetSystemTime();

	// A vector but you can have differently sized classes and structs.
	class ItemVector {
	public:
		ItemVector(int size = 0) {
			if (size != 0) {
				m_data = (char*)malloc(size);
				if (m_data)
					m_maxSize = size;
				else
					log::out << log::RED << "failed allocating memory\n";
			}
		}

		template<class T>
		void writeMemory(char type, void* ptr) {
			int itemSize = sizeof(T);
			if (m_maxSize < m_writeIndex + sizeof(char) + itemSize) {
				if (m_maxSize == 0)
					m_maxSize = 5;

				if(m_maxSize*2< m_writeIndex+sizeof(char)+itemSize) {
					m_maxSize += (sizeof(char) + itemSize)*2;
				} else {
					m_maxSize *= 2;
				}

				char* newPtr = (char*)realloc(m_data, m_maxSize);
				if (newPtr) {
					m_data = newPtr;
				} else {
					m_maxSize = 0;
					log::out << log::RED << "failed reallocating memory\n";
					return;
				}
			}
			*(m_data + m_writeIndex) = type;
			std::memcpy(m_data + m_writeIndex + sizeof(char), ptr, itemSize);
			m_writeIndex += sizeof(char) + itemSize;
		}
		char readType() {
			if (m_writeIndex < m_readIndex + sizeof(char)) {
				return 0;
			}

			char type = *(m_data + m_readIndex);
			m_readIndex += sizeof(char);
			return type;
		}
		template<class T>
		T* readItem() {
			if (m_writeIndex < m_readIndex + sizeof(T)) {
				log::out << "reached end of HeapMemory\n";
				return nullptr;
			}

			char* ptr = m_data + m_readIndex;
			m_readIndex += sizeof(T);
			return (T*)ptr;
		}
		void clear() {
			m_writeIndex = 0;
			m_readIndex = 0;
		}
	private:
		char* m_data = nullptr;
		uint32_t m_maxSize = 0;
		uint32_t m_writeIndex = 0;
		uint32_t m_readIndex = 0;
	};
	class LinearAllocator {
	public:
		LinearAllocator(uint32_t size = 0);
		LinearAllocator(char* stackPtr, uint32_t size);
		~LinearAllocator();

		// A resize will clear the arena and allocate a new block
		// returns whether succesful or not, false means failed
		bool resize(uint32_t size);
		void clear();
		// Returns nullptr if failed.
		char* allocate(uint32_t size);
		template<typename T>
		T* allocate() {
			return allocate(sizeof(T));
		}

	private:
		uint32_t m_size=0;
		char* m_data=nullptr;
		uint32_t m_head=0;
		bool m_stackAllocation=false;
	};

	class Timer {
	public:
		Timer();
		Timer(const std::string& str);
		Timer(const std::string& str, int id);
		~Timer();
		void end();

	private:
		double time;
		std::string name;
		int id = 0;
	};

	#define TIMER(str) Timer str = Timer(#str,__LINE__*strlen(__FILE__))
	
	std::string GetClock();
	float GetRandom();

	//void CountingTest(int times, int numElements, std::function<int()> func);

	template<typename Base, typename T>
	inline bool instanceof(const T*) {
		return std::is_base_of<Base, T>::value;
	}

	template<typename Base, typename T>
	inline Base instance(const T*) {
		return std::is_base_of<Base, T>;
	}
	bool FindFile(const std::string& path);
	std::vector<std::string> GetFiles(const std::string& path);
}