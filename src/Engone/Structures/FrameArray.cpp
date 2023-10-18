#include "Engone/Structures/FrameArray.h"

namespace engone {
	FrameArray::FrameArray(u32 typeSize, u32 valuesPerFrame, u32 allocType) :
		m_typeSize(typeSize), m_valuesPerFrame(valuesPerFrame), m_frames(sizeof(Frame), allocType) {
		//u32 off = valuesPerFrame & 63;
		//if(off!=0)
		//	m_valuesPerFrame += 64 - off;
		u32 off = valuesPerFrame & 7;
		if (off != 0)
			m_valuesPerFrame += 8 - off;
	}
	void FrameArray::cleanup() {
		for (int i = 0; i < m_frames.max; i++) {
			Frame& frame = *((Frame*)m_frames.data + i);
			//if (frame.memory.max != 0) {
			//	for (int j = 0; j < m_valuesPerFrame; j++) {
			//		bool yes = frame.getBool(j);
			//		if (yes) {
			//			Value* ptr = frame.getValue(j, m_typeSize, m_valuesPerFrame);
			//			ptr->~Value();
			//		}
			//	}
			//}
			frame.memory.resize(0);
		}
		m_frames.resize(0);
		m_valueCount = 0;
	}
	FrameArray::Frame::Frame(u32 allocType) : memory{1,allocType} {}

	// Returns -1 if something failed
	u32 FrameArray::add(void* value, void** outPtr) {
		if (m_valuesPerFrame == 0) {
			log::out << log::RED << "FrameArray : valuesPerFrame is 0\n";
			return -1;
		}
		//if(m_valuesPerFrame !=8)
		//	log::out << log::Disable;

		//log::out << "FA : ADD "<<m_valuesPerFrame<<"\n";
		// Find available frame
		int frameIndex = -1;
		for (int i = 0; i < m_frames.max; i++) {
			Frame& frame = *((Frame*)m_frames.data + i);
			//log::out << "FA : " << frame.count << " != " << m_valuesPerFrame<< "\n";
			if (frame.count != m_valuesPerFrame) {
				frameIndex = i;
				//log::out << "FA : found frame "<<i<<" with "<< frame.count << " objects\n";
				break;
			}
		}
		// Create new frame if non found
		if (frameIndex == -1) {
			int initialMax = m_frames.max;
			bool yes = m_frames.resize(m_frames.max * 1.5 + 1);
			if (!yes) {
				//log::out >> log::Disable;
				log::out << log::RED << "FrameArray : failed resize frames\n";
				return -1;
			}
			// memset((Frame*)m_frames.data + initialMax, 0, (m_frames.max - initialMax) * sizeof(Frame));
			frameIndex = initialMax;
			for (int i = initialMax; i < m_frames.max; i++) {
				*((Frame*)m_frames.data + i) = Frame(m_frames.getAllocType()); // char
			}
			//log::out << "FA : Create frame " << frameIndex << "\n";
		}

		// Insert value into frame
		Frame& frame = *((Frame*)m_frames.data + frameIndex);
		if (frame.memory.max == 0) {
			bool yes = frame.memory.resize(m_valuesPerFrame + m_valuesPerFrame * m_typeSize);
			//bool yes = frame.memory.resize(m_valuesPerFrame/8+ m_valuesPerFrame * sizeof(Value));
			if (!yes) {
				log::out << log::RED << "FrameArray : failed resize frame\n";
				//log::out >> log::Disable; 
				return -1;
			}

			//memset(frame.memory.data,0,m_valuesPerFrame/8);
			memset(frame.memory.data,0,m_valuesPerFrame);
			//log::out << "FA : Reserve values\n";
		}

		// Find empty slot
		int valueIndex = -1;
		for (int i = 0; i < m_valuesPerFrame; i++) {
			bool yes = frame.getBool(i);
			if (!yes) {
				valueIndex = i;
				//log::out << "FA : found spot " << i << "\n";
				break;
			} else {
				//log::out << "FA : checked spot " << i <<"\n";
			}
		}

		if (valueIndex == -1) {
			log::out << log::RED << "FrameArray : Impossible error adding value in frame " << frameIndex << "\n";
			//log::out >> log::Disable;
			return -1;
		}
		frame.count++;
		//log::out << "Frame : New object count " << frame.count << "\n";
		m_valueCount++;
		frame.setBool(valueIndex, true);
		void* ptr = frame.getValue(valueIndex, m_typeSize, m_valuesPerFrame);
		//new(ptr)Value(value);
		if (value)
			memcpy(ptr,value,m_typeSize);
		else
			memset(ptr, 1, m_typeSize);
		//log::out >> log::Disable;
		if (outPtr)
			*outPtr = ptr;
		//log::out << "FA:Add " << valueIndex << "\n";
		return frameIndex * m_valuesPerFrame + valueIndex;
	}
	void* FrameArray::get(u32 index) {
		u32 frameIndex = index / m_valuesPerFrame;
		u32 valueIndex = index % m_valuesPerFrame;

		if (frameIndex >= m_frames.max)
			return nullptr;

		Frame& frame = *((Frame*)m_frames.data + frameIndex);

		if (valueIndex >= frame.memory.max)
			return nullptr;

		bool yes = frame.getBool(valueIndex);
		if (!yes) // Check if slot is empty
			return nullptr;

		void* ptr = frame.getValue(valueIndex, m_typeSize, m_valuesPerFrame);
		return ptr;
	}
	void FrameArray::remove(u32 index) {
		u32 frameIndex = index / m_valuesPerFrame;
		u32 valueIndex = index % m_valuesPerFrame;

		if (frameIndex >= m_frames.max)
			return;

		Frame& frame = *((Frame*)m_frames.data + frameIndex);

		if (frame.memory.max == 0)
			return;
		bool yes = frame.getBool(valueIndex);
		if (!yes)
			return;

		frame.count--;
		m_valueCount--;
		frame.setBool(valueIndex,false);
		//void* ptr = frame.getValue(valueIndex, m_valuesPerFrame);
		//ptr->~Value();

		//if (frame.count==0) {
		//	frame.resize(0);
		//}
	}
	u32 FrameArray::getMemoryUsage() {
		uint32_t bytes = m_frames.max * sizeof(Frame);
		for (int i = 0; i < m_frames.max; i++) {
			bytes += ((Frame*)m_frames.data + i)->memory.max;
		}
		return bytes;
	}
	bool FrameArray::iterate(Iterator& iterator) {
		while (true) {
			u32 frameIndex = iterator.position / m_valuesPerFrame;
			if (frameIndex >= m_frames.max)
				break;
			Frame& frame = *((Frame*)m_frames.data + frameIndex);

			u32 valueIndex = iterator.position % m_valuesPerFrame;
			if (valueIndex >= frame.memory.max)
				break;

			iterator.position++;

			bool yes = frame.getBool(valueIndex);
			if (!yes)
				continue;

			iterator.ptr = frame.getValue(valueIndex, m_typeSize, m_valuesPerFrame);
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
	// Memory<char> memory{};
	void* FrameArray::Frame::getValue(u32 index, u32 typeSize, u32 vpf) {
		//return (Value*)(memory.data+ vpf/8+index*sizeof(Value));
		return (void*)((char*)memory.data + vpf + index * typeSize);
	}
	bool FrameArray::Frame::getBool(u32 index) {
		//u32 i = index / 8;
		//u32 j = index % 8;
		//char byte = memory.data[i];
		//char bit = byte&(1<<j);

		return *((char*)memory.data + index);
	}
	void FrameArray::Frame::setBool(u32 index, bool yes) {
		//u32 i = index / 8;
		//u32 j = index % 8;
		//if (yes) {
		//	memory.data[i] = memory.data[i] | (1 << j);
		//} else {
		//	memory.data[i] = memory.data[i] & (~(1 << j));
		//}
		*((char*)memory.data + index) = yes;
	}

	void FrameArrayTest() {
		struct Apple {
			float x, y, size;
			void print() {
				printf("{%f, %f, %f}\n", x,y,size);
			}
		};
		FrameArray arr(sizeof(Apple),80,ALLOC_TYPE_HEAP);
		Apple tmp{ 25,15,92 };
		u32 id_a = arr.add(&tmp);
		tmp = { 25,15,92 };
		u32 id_b = arr.add(&tmp);
		
		arr.remove(id_a);//

		Apple* first = (Apple*)arr.get(0);

		printf("%p\n", first);
		tmp = { 92,61,22 };
		int id_c = arr.add(&tmp);

		//printf("%d\n", id_c);
		//a->print();
		//b->print();
		
		FrameArray::Iterator iterator;
		while (arr.iterate(iterator)) {
			((Apple*)iterator.ptr)->print();
		}


		std::cin.get();
	}
}