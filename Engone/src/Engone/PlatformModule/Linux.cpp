#include "Engone/PlatformModule/PlatformLayer.h"

// Todo: Compile for Linux and test the functions
#ifdef _linux_

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

namespace engone {
#define NS 1000000000
	inline TimePoint StartTime() {
		timespec ts;
		int res = clock_gettime(CLOCK_MONOTONIC,&ts);
		if (res == -1) {
			// err
		}
		return ((uint64)ts.tv_sec*NS + (uint64)ts.tv_nsec);
	}
	static bool once = false;
	static uint64 frequency;
	inline double EndTime(TimePoint startPoint) {
		timespec ts;
		int res = clock_gettime(CLOCK_MONOTONIC, &ts);
		if (res == -1) {
			// err
		}
		return (double)(((uint64)ts.tv_sec * NS + (uint64)ts.tv_nsec) - startPoint);
	}
	APIFile OpenFile(const std::string& path, uint64* outFileSize) {
		int fd = open(path.c_str(), O_RDWR|O_CREAT);
		if (fd == -1) {
			printf("[LinuxError %d]\n", errno);
			return 0;
		}

		off_t size = lseek(fd,0,SEEK_END);
		
		if (size == -1) {
			printf("[LinuxError %d]\n", errno);
			size = 0;
		}
		*outFileSize = (uint64)size;
		// fd as 0 would be a valid file descriptor and -1 is invalid
		// hence we +1 that way 0 is invalid.
		return (uint64)(fd+1);
	}
	uint64 ReadFile(APIFile file, char* buffer, uint64 readBytes) {
		ssize_t size = read((int)(file-1), buffer, readBytes);
		if (size == -1) {
			printf("[LinuxError %d]\n", errno);
			return -1;
		}
		return size;
	}
	void CloseFile(APIFile file) {
		if (file)
			close((int)(file - 1));
	}
	static std::mutex s_allocStatsMutex;
	static uint64 s_totalAllocatedBytes = 0;
	static uint64 s_totalNumberAllocations = 0;
	static uint64 s_allocatedBytes = 0;
	static uint64 s_numberAllocations = 0;
	void* Allocate(uint64 bytes) {
		if (bytes == 0) return nullptr;
		void* ptr = malloc(bytes);
		if (!ptr) return nullptr;

		s_allocStatsMutex.lock();
		s_allocatedBytes += bytes;
		s_numberAllocations++;
		s_totalAllocatedBytes += bytes;
		s_totalNumberAllocations++;
		s_allocStatsMutex.unlock();

		return ptr;
	}
	void Free(void* ptr, uint64 bytes) {
		if (!ptr) return;
		free(ptr);

		s_allocStatsMutex.lock();
		s_allocatedBytes -= bytes;
		s_numberAllocations--;
		s_allocStatsMutex.unlock();
	}
	uint64 GetTotalAllocatedBytes() {
		return s_totalAllocatedBytes;
	}
	uint64 GetTotalNumberAllocations() {
		return s_totalNumberAllocations;
	}
	uint64 GetAllocatedBytes() {
		return s_allocatedBytes;
	}
	uint64 GetNumberAllocations() {
		return s_numberAllocations;
	}
}
#endif