#pragma once

// replacement for std malloc, realloc and free.
// useful if you want to debug, change or track something.

// Not that with ALLOC_DELETE you cannot use a class with a namespace.
// 'ALLOC_DELETE(rp3d::PhysicsCommon,common)' will give 'common->rp3d::PhysicsCommon' which isn't allowed.

// #include <cstdlib>
#include <stdlib.h>

#include "Engone/PlatformLayer.h"

#define ALLOC_NEW(CLASS) new((CLASS*)engone::Allocate(sizeof(CLASS))) CLASS
#define ALLOC_DELETE(CLASS,VAR) {VAR->~CLASS();engone::Free(VAR,sizeof(CLASS));}

namespace engone {
	
	// Todo: MemoryAllocator for game state memory
	
	// namespace alloc {
	// 	void* malloc(uint64 size);
	// 	// oldSize is used for tracking. you usually keep it somewhere.
	// 	void* realloc(void* ptr, uint64 oldSize, uint64 newSize);
	// 	// size is used for tracking. you usually keep it somewhere.
	// 	void free(void* ptr, uint64 size);
	// 	uint64 allocatedBytes();
	// }
	struct Memory {
		Memory(uint32 typeSize) : m_typeSize(typeSize) {}

		uint64 max = 0;
		uint64 used = 0; // may be useful to you.
		void* data = nullptr;

		// count is not in bytes.
		// function is only defined for Memory<char>
		bool resize(uint64 count);

	private:
		uint32 m_typeSize = 0;
	};
	// Does not have a destructor. You need to free the memory with resize(0)
	// template<class T>
	// struct Memory {
	// 	uint64 max = 0;
	// 	uint64 used = 0; // may be useful to you.
	// 	T* data = nullptr;

	// 	// count is not in bytes.
	// 	bool resize(uint64 count) {
	// 		return ((Memory<char>*)this)->resize(sizeof(T), count);
	// 	}
	// 	// function is only defined for Memory<char>
	// 	bool resize(uint64 count, uint64 resize);
	// };
}