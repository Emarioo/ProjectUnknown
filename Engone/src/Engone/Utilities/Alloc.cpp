#include "Engone/Utilities/Alloc.h"

// #include "Engone/Logger.h"

// #include <mutex>

namespace engone {
	namespace alloc {
		// Todo: Thread safety for alloc
		// static std::mutex s_mutex;
		static uint64 s_allocatedBytes=0;

		void* malloc(uint64 size) {
			if (size == 0) return nullptr;
			void* ptr = std::malloc(size);
			if (ptr) {
				// s_mutex.lock();
				s_allocatedBytes += size;
				// s_mutex.unlock();
				//log::out << "Allocate: " << size << "("<<s_allocatedBytes<<")\n";
			}
			return ptr;
		}
		void* realloc(void* ptr, uint64 oldSize, uint64 newSize) {
			if (newSize == 0) {
				alloc::free(ptr, oldSize);
				return nullptr;
			}
			void* newPtr = std::realloc(ptr, newSize);
			if (newPtr) {
				// s_mutex.lock();
				s_allocatedBytes -= oldSize;
				s_allocatedBytes += newSize;
				// s_mutex.unlock();
				return newPtr;
			} else {
				return ptr;
			}
		}
		void free(void* ptr, uint64 size) {
			if (!ptr) return;
			// s_mutex.lock();
			s_allocatedBytes -= size;
			// s_mutex.unlock();
			std::free(ptr);
		}
		uint64 allocatedBytes() {
			return s_allocatedBytes; // reading doesn't really require a mutex
		}
	}
	template<>
	bool Memory<char>::resize(uint64 size, uint64 count) {
		if (count == 0) {
			if (data)
				alloc::free(data, max * size);
			data = nullptr;
			max = 0;
			used = 0;
		}
		if (!data) {
			data = (char*)alloc::malloc(count * size);
			if (data) {
				max = count;
				used = 0;
			}
		} else {
			char* newData = (char*)alloc::realloc(data, max * size, count * size);
			if (newData) {
				data = newData;
				max = count;
				if (max < used)
					used = max;
			}
		}
		//printf("Resize max: %d count: %d\n", max,count);
		return max == count; // returns true when intention was successful
	}
}