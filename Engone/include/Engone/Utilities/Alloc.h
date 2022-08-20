#pragma once

// replacement for std malloc, realloc and free.
// useful if you want to debug, change or track something.

namespace engone {
	namespace alloc {
		void* malloc(uint64_t size);
		// oldSize is used for tracking. you usually keep it somewhere.
		void* realloc(void* ptr, uint64_t oldSize, uint64_t newSize);
		// size is used for tracking. you usually keep it somewhere.
		void free(void* ptr, uint64_t size);
		uint32_t allocatedBytes();
	}
}