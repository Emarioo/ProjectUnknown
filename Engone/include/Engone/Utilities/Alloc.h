#pragma once

// replacement for std malloc, realloc and free.
// useful if you want to debug, change or track something.

// Not that with ALLOC_DELETE you cannot use a class with a namespace.
// 'ALLOC_DELETE(rp3d::PhysicsCommon,common)' will give 'common->rp3d::PhysicsCommon' which isn't allowed.

#define ALLOC_NEW(CLASS) new((CLASS*)engone::alloc::malloc(sizeof(CLASS))) CLASS
#define ALLOC_DELETE(CLASS,VAR) {VAR->~CLASS();engone::alloc::free(VAR,sizeof(CLASS));}

namespace engone {
	namespace alloc {
		void* malloc(uint64_t size);
		// oldSize is used for tracking. you usually keep it somewhere.
		void* realloc(void* ptr, uint64_t oldSize, uint64_t newSize);
		// size is used for tracking. you usually keep it somewhere.
		void free(void* ptr, uint64_t size);
		uint32_t allocatedBytes();
	}
	// Does not have a destructor. You need to free the memory with resize(0)
	template<class T>
	struct Memory {
		uint32_t max = 0;
		uint32_t used = 0; // may be useful to you.
		T* data = nullptr;

		// count is not in bytes.
		bool resize(uint32_t count) {
			return ((Memory<char>*)this)->resize(sizeof(T), count);
		}
		// function is only defined for Memory<char>
		bool resize(uint32_t size, uint32_t count);
	};
}