#pragma once

#include "Engone/Logger.h"
#include "Engone/LoopInfo.h"
#include "Engone/Utilities/Tracker.h"

#include "Engone//Utilities/FileUtility.h"
#include "Engone//Utilities/TimeUtility.h"
#include "Engone//Utilities/RandomUtility.h"
#include "Engone//Utilities/ImageUtility.h"
#include "Engone/Utilities/Alloc.h"

namespace engone {

	std::vector<std::string> SplitString(std::string text, std::string delim);
	//std::string SanitizeString(std::string s);
	float lerp(float a, float b, float c);
	float inverseLerp(float min, float max, float x);
	float distance(float x, float y, float x1, float y1);
	//void insert(float* ar, int off, int len, float* data);
	float bezier(float x, float xStart, float xEnd);
	//std::string Crypt(const std::string& word,const std::string& key, bool encrypt);
	// differnce between two angles, if difference is large clockwise then difference will be negative.
	float AngleDifference(float a, float b);
	//void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3);

	// A vector but you can have differently sized classes and structs.
	class ItemVector {
	public:
		ItemVector(int size = 0);
		~ItemVector();

		template<class T>
		void writeMemory(char type, void* ptr) {
			int itemSize = sizeof(T);
			if (m_maxSize < m_writeIndex + sizeof(char) + itemSize) {
				if (!resize((m_maxSize + sizeof(char) + itemSize) * 2)) {
					return;
				}
			}

			*(m_data + m_writeIndex) = type;
			m_writeIndex += sizeof(char);
			memcpy_s(m_data + m_writeIndex, m_maxSize - m_writeIndex, ptr, itemSize);
			m_writeIndex += itemSize;
		}
		// Returns 0 if end is reached
		char readType();
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
		// true if there are NO items
		bool empty() const;
		// reset read and write index.
		void clear();
		// will reset the class making it as good as new.
		void cleanup();

		static TrackerId trackerId;
	private:
		char* m_data = nullptr;
		uint32_t m_maxSize = 0;
		uint32_t m_writeIndex = 0;
		uint32_t m_readIndex = 0;

		bool resize(uint32_t size);
	};

	// start will activate the class, run should be inside update loop which will update
	// the class's inner timer. Once reached, run will return true once otherwise false.
	// use start again to restart.
	class DelayCode {
	public:
		DelayCode() = default;

		void start(double waitInSeconds) { waitTime = waitInSeconds; running = true; }
		void stop() { running = false; }
		// info is here incase you want to slow down the entire app.
		// Without info, this class would not be affected.
		bool run(UpdateInfo& info) {
			if (running) { 
				time += info.timeStep;
				if (time >= waitTime) {
					running = false;
					return true;
				} 
			}
			return false; 
		}

	private:
		bool running = false;
		double time = 0;
		double waitTime = 0;
	};

	//void CountingTest(int times, int numElements, std::function<int()> func);

	// Starts an exe at path. Uses CreateProcess from windows.h
	bool StartProgram(const std::string& path);
}