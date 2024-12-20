#pragma once

#include <string>

// Todo: Thread safety
// Todo: Optimize string manipulation

// Todo: Get last modified date of a file
// Todo: Check if file exists.

// #define Assert(expression) if(!(expression)) {printf("[Assert] %s (%s:%u)\n",#expression,__FILE__,__LINE__);*((char*)0) = 0;}
#define Assert(X) ((X) ? 0 : ( fprintf(stderr,"[Assert] %s (%s:%u)\n",#X,__FILE__,__LINE__), *((char*)0) = 0))

#define PL_NAMESPACE engone

#define PL_PRINT_ERRORS

#define MegaBytes(x) (x*1024llu*1024llu)
#define GigaBytes(x) (x*1024llu*1024llu*1024llu)

namespace PL_NAMESPACE {
	typedef void APIFile;
	typedef void DirectoryIterator;
	typedef void RecursiveDirectoryIterator;
	struct DirectoryIteratorData{
		std::string name;
		u64 fileSize;
		double lastWriteSeconds;
		bool isDirectory;
	};
	typedef u64 TimePoint;
	
	void* Allocate(u64 bytes);
    void* Reallocate(void* ptr, u64 oldBytes, u64 newBytes);
	void Free(void* ptr, u64 bytes);
	// These are thread safe
	// Successful calls to Allocate
	u64 GetTotalNumberAllocations();
	// Bytes from successful calls to Allocate
	u64 GetTotalAllocatedBytes();
	// Currently allocated bytes
	u64 GetAllocatedBytes();
	// Current allocations
	u64 GetNumberAllocations();
	
	TimePoint MeasureSeconds();
	// returns time in seconds
	double StopMeasure(TimePoint timePoint);
    
    // Note that the platform/os may not have the accuracy you need.
    void Sleep(double seconds);
	
	enum FileFlag : u32{
		FILE_NO_FLAG=0,
		FILE_ONLY_READ=1,
		FILE_CAN_CREATE=2,
		FILE_WILL_CREATE=4,
	};

	// Returns 0 if function failed
    // canWrite = true -> WRITE and READ. False only READ.
	APIFile* FileOpen(const std::string& path, u64* outFileSize = nullptr, u32 flags = FILE_NO_FLAG);
	// Returns number of bytes read
	// -1 means error with read
	u64 FileRead(APIFile* file, void* buffer, u64 readBytes);
	// @return Number of bytes written. -1 indicates an error
	u64 FileWrite(APIFile* file, const void* buffer, u64 writeBytes);
	// @return True if successful, false if not
	// position as -1 will move the head to end of file.
	bool FileSetHead(APIFile* file, u64 position);
	void FileClose(APIFile* file);
    
    bool FileExist(const std::string& path);
    bool DirectoryExist(const std::string& path);
	bool DirectoryCreate(const std::string& path);
    bool FileLastWriteSeconds(const std::string& path, double* seconds);
    
	// Not thread safe if you change working directory or if used within shared libraries. (from msdcindows doc)
	// string is 
	std::string GetWorkingDirectory();
	
	bool FileDelete(const std::string& path);
	bool FileCopy(const std::string& src, const std::string& dst);
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
    
	void SetConsoleColor(u16 color);

	struct PlatformError{
		u32 errorType;
		std::string message; // fixed size?
	};
	bool PollError(PlatformError* out);
	void TestPlatformErrors();

    typedef u32 ThreadId;
	class Thread {
	public:
		Thread() = default;
		~Thread();
		// The thread itself should not call this function
		void cleanup();
		
		void init(u32(*func)(void*), void* arg);
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
		u32 m_initial = 1;
		u32 m_max = 1;
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
	
	// Starts an exe at path. Uses CreateProcess from windows.h
	// commandLine cannot be constant (CreateProcessA in windows api says so)
	bool StartProgram(const std::string& path, char* commandLine=NULL);

	typedef void(*VoidFunction)();
	// @return null on error (library not found?). Pass returned value into GetFunctionAdress to get function pointer. 
	void* LoadDynamicLibrary(const std::string& path);
	void UnloadDynamicLibrary(void* library);
	// You may need to cast the function pointer to the appropriate function
	VoidFunction GetFunctionPointer(void* library, const std::string& name);

	// Converts arguments from WinMain into simpler arguments. Not unicode.
	// note that argc and argv are references and the outputs of this function.
	// do not forget to call FreeArguments because this function allocates memory.
	void ConvertArguments(int& argc, char**& argv);
	// same as previous but arguments are converted from paramteter args.
	// args is what would follow when you call the executable. Ex, mygame.exe --console --server
	void ConvertArguments(const char* args, int& argc, char**& argv);
	void FreeArguments(int argc, char** argv);
	// calls AllocConsole and sets stdin and stdout
	void CreateConsole();

	// Monitor a directory or file where any changes to files will call the callback with certain path.
	class FileMonitor {
	public:
		//-- flags
		enum Flags : u32 {
			WATCH_SUBTREE = 1,
		};
		enum ChangeType : u32 {
			FILE_REMOVED = 1,
			FILE_MODIFIED = 2, // includes added file
		};

		FileMonitor() = default;
		~FileMonitor();
		void cleanup();

		// path can be file or directory
		// calling this again will restart the tracking with new arguments.
		// the argument in the callback is a relative path from root to the file that was changed.
		// returns false if root path was invalid
		bool check(const std::string& root, void (*callback)(const std::string&, u32), u32 flags = 0);
		// bool check(const std::string& root, std::function<void(const std::string&, u32)> callback, u32 flags = 0);

		inline const std::string& getRoot() { return m_root; }

		inline void (*getCallback())(const std::string&, u32) { return m_callback; }
		// inline std::function<void(const std::string&, u32)>& getCallback() { return m_callback; }

	private:
		bool m_running = false;
		// std::function<void(const std::string&, u32)> m_callback;
		void (*m_callback)(const std::string&, u32);
		
		std::string m_root; // path passed to check function
		std::string m_dirPath; // if m_root isn't a directory then this will be the dir of the file
		u32 m_flags = 0;

		void* m_changeHandle=NULL;
		Mutex m_mutex;
		Thread m_thread;
		//HANDLE m_threadHandle = NULL; // used to cancel ReadDirectoryChangesW
	
		void* m_dirHandle = NULL;
		void* m_buffer=nullptr;
		u32 m_bufferSize=0;
	
		friend u32 RunMonitor(void* arg);
	};
	
	// You set a threads name with it. Use -1 as threadId if you want to set the name for the current thread.
	// This only works in visual studios debugger.
	// An alternative is SetThreadDescription but it is not available in Windows 8.1 which I am using.
	void SetThreadName(ThreadId threadId, const char* threadName);
}