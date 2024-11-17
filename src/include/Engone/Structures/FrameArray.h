#pragma once

#include "Engone/Utilities/Alloc.h"

#include "Engone/Logger.h"
namespace engone {
	
	// IMPORTANT
	class FrameArray {
	public:
		// constructor does nothing except remember the variable
		// valuesPerFrame is forced to be divisible by 8. (data alignment * bits as bools)
		FrameArray(u32 typeSize, u32 valuesPerFrame, u32 allocType);
		~FrameArray() { cleanup(); }
		// Does not call free on the items.
		void cleanup();
		
		// Returns -1 if something failed
		// Does not initialize the item
		// @param value pointer to an item which should be added, note that a memcpy occurs. value can be nullptr.
		// @param outPtr sets the pointer for the added object
		u32 add(void* value, void** outPtr = nullptr);
		
		void* get(u32 index);
		void remove(u32 index);
		// in bytes
		u32 getMemoryUsage();
		// YOU CANNOT USE THIS VALUE TO ITERATE THROUGH THE ELEMENTS!
		inline u32 getCount() {return m_valueCount;}
		struct Iterator {
			u32 position=0;
			void* ptr=nullptr;
		};
		// Returns false if nothing more to iterate. Values in iterator are reset
		bool iterate(Iterator& iterator);
		// max = x/8+x*C
		// max = x*(1/8+C)
		// max/(1/8+C)
		// 8*max / (1+8*C)
	private:
		struct Frame {
			Frame(u32 allocType);
			// Memory<char> memory{};
			Memory memory;
			int count = 0;
			void* getValue(u32 index, u32 typeSize, u32 vpf);
			bool getBool(u32 index);
			void setBool(u32 index, bool yes);
		};

		Memory m_frames;
		int m_valueCount = 0;
		
		u32 m_valuesPerFrame = 0;
		u32 m_typeSize = 0;
	};

	void FrameArrayTest();
}