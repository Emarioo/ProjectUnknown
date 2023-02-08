#include "Engone/Utilities/Alloc.h"

#include "Engone/Logger.h"

namespace engone {
	namespace alloc {
		static std::mutex s_mutex;
		static uint32_t s_allocatedBytes=0;

		void* malloc(uint64_t size) {
			if (size == 0) return nullptr;
			void* ptr = std::malloc(size);
			if (ptr) {
				s_mutex.lock();
				s_allocatedBytes += size;
				s_mutex.unlock();
				//log::out << "Allocate: " << size << "("<<s_allocatedBytes<<")\n";
			}
			return ptr;
		}
		void* realloc(void* ptr, uint64_t oldSize, uint64_t newSize) {
			if (newSize == 0) {
				alloc::free(ptr, oldSize);
				return nullptr;
			}
			void* newPtr = std::realloc(ptr, newSize);
			if (newPtr) {
				s_mutex.lock();
				s_allocatedBytes -= oldSize;
				s_allocatedBytes += newSize;
				s_mutex.unlock();
				return newPtr;
			} else {
				return ptr;
			}
		}
		void free(void* ptr, uint64_t size) {
			if (!ptr) return;
			s_mutex.lock();
			s_allocatedBytes -= size;
			s_mutex.unlock();
			std::free(ptr);
		}
		uint32_t allocatedBytes() {
			return s_allocatedBytes; // reading doesn't really require a mutex
		}
	}
	template<>
	bool Memory<char>::resize(uint32_t size, uint32_t count) {
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