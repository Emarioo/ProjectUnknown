#include "Alloc.h"

namespace engone {
	namespace alloc {
		static uint32_t s_allocatedBytes=0;
		void* malloc(uint64_t size) {
			if (size == 0) return nullptr;
			void* ptr = std::malloc(size);
			if(ptr)
				s_allocatedBytes += size;
			return ptr;
		}
		void* realloc(void* ptr, uint64_t oldSize, uint64_t newSize) {
			if (newSize == 0) {
				free(ptr, oldSize);
				return nullptr;
			}
			void* newPtr = std::realloc(ptr, newSize);
			if (newPtr) {
				s_allocatedBytes -= oldSize;
				s_allocatedBytes += newSize;
				return newPtr;
			} else {
				return ptr;
			}
		}
		void free(void* ptr, uint64_t size) {
			if (!ptr) return;
			s_allocatedBytes -= size;
			std::free(ptr);
		}
		uint32_t allocatedBytes() {
			return s_allocatedBytes;
		}
	}
}