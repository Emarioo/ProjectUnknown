#pragma once

#include "Engone/Utilities/Alloc.h"

#include "Engone/Logger.h"
namespace engone {
	
	template<class Value>
	class FrameArray {
	public:
		//typedef int Value;

		// constructor does nothing except remember the variable
		// valuesPerFrame is forced to be divisible by 64. (data alignment * bits as bools)
		FrameArray(uint32_t valuesPerFrame) : m_valuesPerFrame(valuesPerFrame) {
			uint32_t off = valuesPerFrame & 63;
			if(off!=0)
				m_valuesPerFrame += 64 - off;
		}
		~FrameArray() {
			cleanup();
		}
		void cleanup() {
			for (int i = 0; i < m_frames.max;i++) {
				Frame& frame = m_frames.data[i];
				if (frame.memory.max != 0) {
					for (int j = 0; j < m_valuesPerFrame; j++) {
						bool yes = frame.getBool(j);
						if (yes) {
							Value* ptr = frame.getValue(j, m_valuesPerFrame);
							ptr->~Value();
						}
					}
				}
				frame.memory.resize(0);
			}
			m_frames.resize(0);
			m_valueCount = 0;
		}
		
		// Returns -1 if something failed
		uint32_t add(Value value) {
			if (m_valuesPerFrame == 0) return -1;

			// Find available frame
			int frameIndex = -1;
			for (int i = 0; i < m_frames.max; i++) {
				Frame& frame = m_frames.data[i];
				if (frame.count != m_valuesPerFrame) {
					frameIndex = i;
					break;
				}
			}
			// Create new frame if non found
			if (frameIndex==-1) {
				int initialMax = m_frames.max;
				bool yes = m_frames.resize(m_frames.max*1.25+1);
				if (!yes) return -1;
				memset(m_frames.data + initialMax, 0, (m_frames.max - initialMax) * sizeof(Frame));
				frameIndex = initialMax;
			}
			
			// Insert value into frame
			Frame& frame = m_frames.data[frameIndex];
			if (frame.memory.max==0) {
				bool yes = frame.memory.resize(m_valuesPerFrame/8+ m_valuesPerFrame * sizeof(Value));
				if (!yes) return -1;

				//memset(frame.memory.data,0,frame.memory.max);
				memset(frame.memory.data,0,m_valuesPerFrame/8);
			}
			
			// Find empty slot
			int valueIndex = -1;
			for (int i = 0; i < m_valuesPerFrame;i++) {
				bool yes = frame.getBool(i);
				if (!yes) {
					valueIndex = i;
					break;
				}
			}

			if (valueIndex == -1) {
				log::out << log::RED << "FrameArray : Impossible error adding value in frame " << frameIndex << "\n";
				return -1;
			}
			frame.count++;
			m_valueCount++;
			frame.setBool(valueIndex, true);
			Value* ptr = frame.getValue(valueIndex, m_valuesPerFrame);
			new(ptr)Value(value);
			return valueIndex;
		}
		
		Value* get(uint32_t index) {
			uint32_t frameIndex = index / m_valuesPerFrame;
			uint32_t valueIndex = index % m_valuesPerFrame;

			if (frameIndex >= m_frames.max)
				return nullptr;

			Frame& frame = m_frames.data[frameIndex];

			if (valueIndex >= frame.memory.max)
				return nullptr;

			bool yes = frame.getBool(valueIndex);
			if (!yes) // Check if slot is empty
				return nullptr;

			Value* ptr = frame.getValue(valueIndex, m_valuesPerFrame);
			return ptr;
		}
		void remove(uint32_t index) {
			uint32_t frameIndex = index / m_valuesPerFrame;
			uint32_t valueIndex = index % m_valuesPerFrame;

			if (frameIndex >= m_frames.max)
				return;

			Frame& frame = m_frames.data[frameIndex];

			if (frame.memory.max==0)
				return;
			bool yes = frame.getBool(valueIndex);
			if (!yes)
				return;
			
			frame.count--;
			m_valueCount--;
			frame.setBool(valueIndex,false);
			Value* ptr = frame.getValue(valueIndex, m_valuesPerFrame);
			ptr->~Value();

			//if (frame.count==0) {
			//	frame.resize(0);
			//}
		}
		// in bytes
		uint32_t getMemoryUsage() {
			uint32_t bytes = m_frames.max*sizeof(Frame);
			for (int i = 0; i < m_frames.max; i++) {
				bytes += m_frames.data[i].memory.max;
			}
			return bytes;
		}
		// YOU CANNOT USE THIS VALUE TO ITERATE THROUGH THE ELEMENTS!
		uint32_t getCount() {
			return m_valueCount;
		}
		struct Iterator {
			uint32_t position=0;
			Value* ptr=nullptr;
		};
		// Returns false if nothing more to iterate. Values in iterator are reset
		bool iterate(Iterator& iterator) {
			while (true) {
				uint32_t frameIndex = iterator.position / m_valuesPerFrame;
				if (frameIndex >= m_frames.max)
					break;
				Frame& frame = m_frames.data[frameIndex];

				uint32_t valueIndex = iterator.position % m_valuesPerFrame;
				if (valueIndex >= frame.memory.max)
					break;
				
				iterator.position++;

				bool yes = frame.getBool(valueIndex);
				if (!yes)
					continue;
				
				iterator.ptr = frame.getValue(valueIndex, m_valuesPerFrame);
				return true;
			}
			iterator.position = 0;
			iterator.ptr = nullptr;
			return false;
		}
		// max = x/8+x*C
		// max = x*(1/8+C)
		// max/(1/8+C)
		// 8*max / (1+8*C)
	private:
		struct Frame {
			Memory<char> memory{};
			int count = 0;
			Value* getValue(uint32_t index, uint32_t vpf) {
				return (Value*)(memory.data+ vpf/8+index*sizeof(Value));
			}
			bool getBool(uint32_t index) {
				uint32_t i = index / 8;
				uint32_t j = index % 8;
				char byte = memory.data[i];
				char bit = byte&(1<<j);
				return bit;
			}
			void setBool(uint32_t index, bool yes) {
				uint32_t i = index / 8;
				uint32_t j = index % 8;
				if (yes) {
					memory.data[i] = memory.data[i] | (1 << j);
				} else {
					memory.data[i] = memory.data[i] & (~(1 << j));
				}
			}
		};

		Memory<Frame> m_frames{};
		int m_valueCount=0;
		
		uint32_t m_valuesPerFrame=0;
	
	};

	void FrameArrayTest();
}