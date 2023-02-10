#pragma once

#include "Engone/Utilities/Typedefs.h"

// When should assert be active? in debug mode?
#define Assert(expression) if(!expression) {printf("[Assert] %s\n",#expression);*((char*)0) = 0;}

namespace engone {
	//-- Platform API
	// APIFile = 0 indicates no file
	typedef uint64 APIFile;
	typedef uint64 TimePoint;

	//-- Files
	// Returns 0 if function failed.
	APIFile OpenFile(const std::string& path, uint64* outFileSize=nullptr);
	// Returns number of bytes read
	uint64 ReadFile(APIFile file, char* buffer, uint64 readBytes);

	void CloseFile(APIFile file);

	//-- Memory
	void* Allocate(uint64 bytes);
	void Free(void* ptr, uint64 bytes);

	// These are thread safe
	// Successful calls to Allocate
	uint64 GetTotalNumberAllocations();
	// Bytes from successful calls to Allocate
	uint64 GetTotalAllocatedBytes();
	// Currently allocated bytes
	uint64 GetAllocatedBytes();
	// Current allocations
	uint64 GetNumberAllocations();

	//-- Time
	TimePoint StartTime();
	double EndTime(TimePoint timePoint);

}