#pragma once

#include <string>

#include "Engone/Utilities/Typedefs.h"

// Todo: Thread safety
// Todo: Optimize string manipulation

// Todo: Get last modified date of a file
// Todo: Check if file exists.

#define Assert(expression) if(!(expression)) {printf("[Assert] %s (%s:%u)\n",#expression,__FILE__,__LINE__);*((char*)0) = 0;}

#define PL_NAMESPACE engone

#define PL_PRINT_ERRORS

namespace PL_NAMESPACE {
	typedef void APIFile;
	typedef void DirectoryIterator;
	typedef void RecursiveDirectoryIterator;
	struct DirectoryIteratorData{
		std::string name;
		uint64 fileSize;
		double lastWriteSeconds;
		bool isDirectory;
	};
	typedef uint64 TimePoint;
	
	void* Allocate(uint64 bytes);
    void* Reallocate(void* ptr, uint64 oldBytes, uint64 newBytes);
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
	
	TimePoint StartTime();
	double EndTime(TimePoint timePoint);
    
    // Note that the platform/os may not have the accuracy you need.
    void Sleep(double seconds);
	
	// Returns 0 if function failed
    // canWrite = true -> WRITE and READ. False only READ.
	APIFile* FileOpen(const std::string& path, uint64* outFileSize = nullptr, bool onlyRead=false);
	// Returns number of bytes read
	// -1 means error with read
	uint64 FileRead(APIFile* file, char* buffer, uint64 readBytes);
	void FileClose(APIFile* file);
    
    bool FileExist(const std::string& path);
    bool DirectoryExist(const std::string& path);
    bool FileLastWriteSeconds(const std::string& path, double* seconds);
    
    // Todo: Remove the simple directory iterator. Skipping directories in the recursive directory iterator
    //      would work the same as the normal directory iterator.
    
	// path should be a valid directory.
	// Empty string assumes working directory.
	// Result must be a valid pointer.
	// Returns 0 if something failed. Maybe invalid path or empty directory.
	DirectoryIterator* DirectoryIteratorCreate(const std::string& path, DirectoryIteratorData* result);
	// Iterator must be valid. Cannot be 0.
	// Returns true when result has valid data of the next file.
	// Returns false if there are no more files or if something failed. Destroy the iterator in any case. 
	bool DirectoryIteratorNext(DirectoryIterator* iterator, DirectoryIteratorData* result);
	// Iterator must be valid. Cannot be 0.
	void DirectoryIteratorDestroy(DirectoryIterator* iterator);
	
	// 0 is returned on failure (invalid path, empty directory)
	// result must be valid memory.
	// path should be a valid directory.
	// empty string as path assumes working directory.
	RecursiveDirectoryIterator* RecursiveDirectoryIteratorCreate(const std::string& path, DirectoryIteratorData* result);
	// false is returned on failure (invalid iterator, no more files/directories)
	bool RecursiveDirectoryIteratorNext(RecursiveDirectoryIterator* iterator, DirectoryIteratorData* result);
	// Skip the latest directory in the internal recursive queue. The latest found directory will be last in the queue.
	void RecursiveDirectoryIteratorSkip(RecursiveDirectoryIterator* iterator);
	void RecursiveDirectoryIteratorDestroy(RecursiveDirectoryIterator* iterator);
    
    typedef uint32 ThreadId;
	class Thread {
	public:
		Thread() = default;
		~Thread();
		// The thread itself should not call this function
		void cleanup();
		
		void init(uint32(*func)(void*), void* arg);
		void join();
		
        // True: Thread is doing stuff or finished and waiting to be joined.
        // False: Thread is not active. Call init to start the thread. 
		bool isActive();
		bool joinable();

		static ThreadId GetThisThreadId();

		ThreadId getId();

	private:
		void* m_internalHandle = 0;
		ThreadId m_threadId=0;
		
		friend class FileMonitor;
	};
    class Semaphore {
	public:
		Semaphore(int initial=1, int maxLocks=1);
		~Semaphore();
		void cleanup();

		void wait();
		// count is how much to increase the semaphore's count by.
		void signal(int count=1);

	private:
		void* m_internalHandle = 0;
		uint32 m_initial = 1;
		uint32 m_max = 1;
	};
	class Mutex {
	public:
		Mutex() = default;
		~Mutex();
		void cleanup();

		void lock();
		void unlock();

		ThreadId getOwner();
	private:
		ThreadId m_ownerThread = 0;
		void* m_internalHandle = 0;
	};
}