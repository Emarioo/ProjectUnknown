
#include "Engone/PlatformModule/PlatformModule.h"

#ifdef WIN32

#include "Engone/Win32Includes.h"

// Temporary solution for name collision
static auto WinReadFile = ReadFile;

namespace engone {
	inline TimePoint StartTime() {
		uint64 tp;
		BOOL success = QueryPerformanceCounter((LARGE_INTEGER*)&tp);
		// if(!success){
		// 	printf("time failed\n");	
		// }
		// Todo: handle err
		return tp;
	}
	static bool once = false;
	static uint64 frequency;
	inline double EndTime(TimePoint startPoint) {
		if (!once) {
			BOOL success = QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
			// if(!success){
			// 	printf("time failed\n");	
			// }
			// Todo: handle err
		}
		TimePoint endPoint;
		BOOL success = QueryPerformanceCounter((LARGE_INTEGER*)&endPoint);
		// if(!success){
		// 	printf("time failed\n");	
		// }
		return (double)(endPoint - startPoint) / (double)frequency;
	}
	APIFile OpenFile(const std::string& path, uint64* outFileSize) {
		HANDLE handle = CreateFileA(path.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (handle == INVALID_HANDLE_VALUE) {
			if (outFileSize) *outFileSize = 0;
			DWORD err = GetLastError();
			if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
				// printf("[WinError %u] Cannot find '%s'\n",err,path.c_str());
			} else if (err == ERROR_ACCESS_DENIED) {
				// printf("[WinError %u] Denied access to '%s'\n",err,path.c_str()); // tried to open a directory?
			} else {
				printf("[WinError %u] Error opening '%s'\n", err,path.c_str());
			}
			return 0;
		} else if (outFileSize){

			DWORD success = GetFileSizeEx(handle, (LARGE_INTEGER*)outFileSize);
			if (!success) {
				// GetFileSizeEx will probably not fail if CreateFile succeeded. But just in case it does.
				DWORD err = GetLastError();
				printf("[WinError %u] Error aquiring file size from '%s'",err,path.c_str());
				*outFileSize = 0;
				Assert(outFileSize)
			}
		}
		return (uint64)handle;
	}
	uint64 ReadFile(APIFile file, char* buffer, uint64 readBytes) {
		DWORD bytesRead = 0;
		DWORD success = WinReadFile((HANDLE)file, buffer, readBytes, &bytesRead, NULL);
		if (!success) {
			DWORD err = GetLastError();
			printf("[WinError %u] Error reading file handle '%llu'\n", err, (uint64)file);
			return -1;
		}
		return bytesRead;
	}
	void CloseFile(APIFile file) {
		if (file)
			CloseHandle((HANDLE)file);
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