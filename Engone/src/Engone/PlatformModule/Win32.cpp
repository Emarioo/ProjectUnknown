#include "Engone/PlatformModule/PlatformLayer.h"

#ifdef WIN32

#ifdef PL_PRINT_ERRORS
#define PL_PRINTF printf
#else
#define PL_PRINTF
#endif

#include <unordered_map>
#include <vector>

// #define NOMINMAX
// #define _WIN32_WINNT 0x0601
// #define WIN32_LEAN_AND_MEAN
// #include <windows.h>

// #include <mutex>

#include "Engone/Win32Includes.h"

// Name collision
auto Win32Sleep = Sleep;

namespace PL_NAMESPACE {
//-- Platform specific

    #define TO_INTERNAL(X) (void*)((uint64)X+1)
    #define TO_HANDLE(X) (HANDLE)((uint64)X-1)

	DirectoryIterator* DirectoryIteratorCreate(const std::string& path, DirectoryIteratorData* result){		
		WIN32_FIND_DATAA data;
		std::string _path;
		if(path.empty())
			_path = path+"*";
		else
			_path = path+"\\*";
		HANDLE handle = FindFirstFileA(_path.c_str(),&data);
		if(handle==INVALID_HANDLE_VALUE){
			DWORD err = GetLastError();
			if(err == ERROR_FILE_NOT_FOUND){
				PL_PRINTF("[WinError %u] Cannot find file '%s'\n",err,path.c_str());
			}else if(err==ERROR_PATH_NOT_FOUND){
				PL_PRINTF("[WinError %u] Cannot find path '%s'\n",err,path.c_str());
			}else {
				PL_PRINTF("[WinError %u] Error opening '%s'\n",err,path.c_str());
			}
			return 0;
		}
		while(true){
			if(strcmp(data.cFileName,".")==0||strcmp(data.cFileName,"..")==0){
				BOOL success = FindNextFileA(handle,&data);
				if(!success){
					DWORD err = GetLastError();
					if(err == ERROR_NO_MORE_FILES){
						// PL_PRINTF("[WinError %u] No files '%lu'\n",err,(uint64)iterator);
					}else {
						PL_PRINTF("[WinError %u] Error iterating '%lu'\n",err,(uint64)handle);
					}
					BOOL success = FindClose(handle);
					if(!success){
						DWORD err = GetLastError();
						PL_PRINTF("[WinError %u] Error closing '%lu'\n",(uint64)handle);
					}
					return 0;
				}
				continue;
			}
			break;
		}
		// PL_PRINTF("F1: %s\n",data.cFileName);
		// PL_PRINTF("F2: %s\n",data.cAlternateFileName);
		result->name = data.cFileName;
		result->fileSize = (uint64)data.nFileSizeLow+(uint64)data.nFileSizeHigh*((uint64)MAXDWORD+1);
		uint64 time = (uint64)data.ftLastWriteTime.dwLowDateTime+(uint64)data.ftLastWriteTime.dwHighDateTime*((uint64)MAXDWORD+1);
		result->lastWriteSeconds = time/10000000.f; // 100-nanosecond intervals
		result->isDirectory = data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
		
		return (DirectoryIterator*)((uint64)handle+1); // +1 in case 0 is a valid handle. Could not find anything about this
	}
	bool DirectoryIteratorNext(DirectoryIterator* iterator, DirectoryIteratorData* result){
		WIN32_FIND_DATAA data;
		while(true){
			BOOL success = FindNextFileA((HANDLE)((uint64)iterator-1),&data);
			if(!success){
				DWORD err = GetLastError();
				if(err == ERROR_NO_MORE_FILES){
					// take one from directory and do shit.
					// PL_PRINTF("[WinError %u] No files '%lu'\n",err,(uint64)iterator);
				}else {
					PL_PRINTF("[WinError %u] Error iterating '%lu'\n",err,(uint64)iterator);
				}
				return false;
			}
			if(strcmp(data.cFileName,".")==0||strcmp(data.cFileName,"..")==0){
				continue;
			}
			break;
		}
		
		result->name = data.cFileName;
		result->fileSize = (uint64)data.nFileSizeLow+(uint64)data.nFileSizeHigh*((uint64)MAXDWORD+1);
		uint64 time = (uint64)data.ftLastWriteTime.dwLowDateTime+(uint64)data.ftLastWriteTime.dwHighDateTime*((uint64)MAXDWORD+1);
		result->lastWriteSeconds = time/10000000.f; // 100-nanosecond intervals
		result->isDirectory = data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
		return true;
	}
	void DirectoryIteratorDestroy(DirectoryIterator* iterator){
		BOOL success = FindClose((HANDLE)((uint64)iterator-1));
		if(!success){
			DWORD err = GetLastError();
			PL_PRINTF("[WinError %u] Error closing '%lu'\n",(uint64)iterator);
		}
	}

	// Recursive directory iterator info
	struct RDIInfo{
		std::string root;
		std::string dir;
		HANDLE handle;
		std::vector<std::string> directories;
	};
	static std::unordered_map<RecursiveDirectoryIterator*,RDIInfo> s_rdiInfos;
	static uint64 s_uniqueRDI=0;
	
	RecursiveDirectoryIterator* RecursiveDirectoryIteratorCreate(const std::string& path, DirectoryIteratorData* result){
		RecursiveDirectoryIterator* iterator = (RecursiveDirectoryIterator*)(++s_uniqueRDI);
		s_rdiInfos[iterator] = {path};
		s_rdiInfos[iterator].handle=INVALID_HANDLE_VALUE;
		s_rdiInfos[iterator].directories.push_back(path);
		
		bool success = RecursiveDirectoryIteratorNext(iterator,result);
		if(!success){
			RecursiveDirectoryIteratorDestroy(iterator);
			return 0;
		}
		return iterator;
	}
	bool RecursiveDirectoryIteratorNext(RecursiveDirectoryIterator* iterator, DirectoryIteratorData* result){
		auto info = s_rdiInfos.find(iterator);
		if(info==s_rdiInfos.end()){
			return false;
		}
		
		WIN32_FIND_DATAA data;
		while(true){
			if(info->second.handle==INVALID_HANDLE_VALUE){
				if(info->second.directories.empty()){
					return false;
				}
                info->second.dir.clear();
                
				if(!info->second.directories[0].empty()){
                    info->second.dir += info->second.directories[0];
				}
                
                std::string temp = info->second.dir;
                if(!temp.empty())
                    temp += "\\";
                
                temp+="*";
				info->second.directories.erase(info->second.directories.begin());
				HANDLE handle = FindFirstFileA(temp.c_str(),&data);
				
				if(handle==INVALID_HANDLE_VALUE){
					// print error?
					continue;
				}
				info->second.handle = handle;
			}else{
				BOOL success = FindNextFileA(info->second.handle,&data);
				if(!success){
					DWORD err = GetLastError();
					if(err == ERROR_NO_MORE_FILES){
						// PL_PRINTF("[WinError %u] No files '%lu'\n",err,(uint64)iterator);
					}else {
						PL_PRINTF("[WinError %u] FindNextFileA '%lu'\n",err,(uint64)iterator);
					}
					bool success = FindClose(info->second.handle);
					info->second.handle = INVALID_HANDLE_VALUE;
					if(!success){
						err = GetLastError();
						PL_PRINTF("[WinError %u] FindClose '%lu'\n",err,(uint64)iterator);
					}
					continue;
				}
			}
			if(strcmp(data.cFileName,".")==0||strcmp(data.cFileName,"..")==0){
				continue;
			}
			break;
		}
		result->name.clear();
		if(!info->second.dir.empty())
			result->name += info->second.dir+"\\";
		result->name += data.cFileName;
		
        // printf("f: %s\n",result->name.c_str());
        
		result->fileSize = (uint64)data.nFileSizeLow+(uint64)data.nFileSizeHigh*((uint64)MAXDWORD+1);
		uint64 time = (uint64)data.ftLastWriteTime.dwLowDateTime+(uint64)data.ftLastWriteTime.dwHighDateTime*((uint64)MAXDWORD+1);
		result->lastWriteSeconds = time/10000000.f; // 100-nanosecond intervals
		result->isDirectory = data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
		if(result->isDirectory){
            info->second.directories.push_back(result->name);
        }
		return true;
	}
	void RecursiveDirectoryIteratorSkip(RecursiveDirectoryIterator* iterator){
		auto info = s_rdiInfos.find(iterator);
		if(info==s_rdiInfos.end()){
			return;
		}
		if(!info->second.directories.empty())
			info->second.directories.pop_back();
	}
	void RecursiveDirectoryIteratorDestroy(RecursiveDirectoryIterator* iterator){
		auto info = s_rdiInfos.find(iterator);
		if(info==s_rdiInfos.end()){
			return;
		}
		
		if(info->second.handle!=INVALID_HANDLE_VALUE){
			BOOL success = FindClose(info->second.handle);
			if(!success){
				DWORD err = GetLastError();
				PL_PRINTF("[WinError %u] Error closing '%lu'\n",(uint64)iterator);
			}
		}
		s_rdiInfos.erase(iterator);
	}

	TimePoint StartTime(){
		uint64 tp;
		BOOL success = QueryPerformanceCounter((LARGE_INTEGER*)&tp);
		// if(!success){
		// 	PL_PRINTF("time failed\n");	
		// }
		// Todo: handle err
		return tp;
	}
	static bool once = false;
	static uint64 frequency;
	double EndTime(TimePoint startPoint){
		if(!once){
			BOOL success = QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
			// if(!success){
			// 	PL_PRINTF("time failed\n");	
			// }
			// Todo: handle err
		}
		TimePoint endPoint;
		BOOL success = QueryPerformanceCounter((LARGE_INTEGER*)&endPoint);
		// if(!success){
		// 	PL_PRINTF("time failed\n");	
		// }
		return (double)(endPoint-startPoint)/(double)frequency;
	}
	void Sleep(double seconds){
        Win32Sleep((uint32)(seconds*1000));   
    }
    APIFile* FileOpen(const std::string& path, uint64* outFileSize, uint32 flags){
        DWORD access = GENERIC_READ|GENERIC_WRITE;
        DWORD sharing = 0;
        if(flags&FILE_ONLY_READ){
            access = GENERIC_READ;
         	sharing = FILE_SHARE_READ;
		}
		DWORD creation = (flags&FILE_CAN_CREATE)?OPEN_ALWAYS:OPEN_EXISTING;
        // printf("creation %u",creation);

		if(creation&OPEN_ALWAYS){
			std::string temp;
			int i=0;
			int at = path.find_first_of(':');
			if(at!=-1){
				i = at+1;
				temp+=path.substr(0,i);
			}
			for(;i<path.length();i++){
				char chr = path[i];
				if(chr=='/'||chr=='\\'){
					// printf("exist %s\n",temp.c_str());
					if(!DirectoryExist(temp)){
						// printf(" create\n");
						bool success = DirectoryCreate(temp);
						if(!success)
							break;
					}
				}
				temp+=chr;
			}
		}

		HANDLE handle = CreateFileA(path.c_str(),access,sharing,NULL,creation,FILE_ATTRIBUTE_NORMAL, NULL);
		
		if(handle==INVALID_HANDLE_VALUE){
			DWORD err = GetLastError();
			if(err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND){
				PL_PRINTF("[WinError %u] Cannot find '%s'\n",err,path.c_str());
			}else if(err == ERROR_ACCESS_DENIED){
				PL_PRINTF("[WinError %u] Denied access to '%s'\n",err,path.c_str()); // tried to open a directory?
			}else {
				PL_PRINTF("[WinError %u] Error opening '%s'\n",err,path.c_str());
			}
			return 0;
		}else if (outFileSize){

			DWORD success = GetFileSizeEx(handle, (LARGE_INTEGER*)outFileSize);
			if (!success) {
				// GetFileSizeEx will probably not fail if CreateFile succeeded. But just in case it does.
				DWORD err = GetLastError();
				printf("[WinError %u] Error aquiring file size from '%s'",err,path.c_str());
				*outFileSize = 0;
				Assert(outFileSize)
			}
		}
		return TO_INTERNAL(handle);
	}
	uint64 FileRead(APIFile* file, void* buffer, uint64 readBytes){
		Assert(readBytes!=(uint64)-1); // -1 indicates no bytes read
		
		DWORD bytesRead=0;
		DWORD success = ReadFile(TO_HANDLE(file),buffer,readBytes,&bytesRead,NULL);
		if(!success){
			DWORD err = GetLastError();
			PL_PRINTF("[WinError %u] Error reading file handle '%lu'\n",err,file);
			return -1;
		}
		return bytesRead;
	}
	uint64 FileWrite(APIFile* file, void* buffer, uint64 writeBytes){
		Assert(writeBytes!=(uint64)-1); // -1 indicates no bytes read
		
		DWORD bytesWritten=0;
		DWORD success = WriteFile(TO_HANDLE(file),buffer,writeBytes,&bytesWritten,NULL);
		if(!success){
			DWORD err = GetLastError();
			PL_PRINTF("[WinError %u] Error reading file handle '%lu'\n",err,file);
			return -1;
		}
		return bytesWritten;
	}
	bool FileSetHead(APIFile* file, uint64 position){
		DWORD success = 0;
		if(position==-1){
			success = SetFilePointerEx(file,{0},NULL,FILE_END);
		}else{
			success = SetFilePointerEx(file,*(LARGE_INTEGER*)&position,NULL,FILE_BEGIN);
		}
		if(success) return true;
		
		int err = GetLastError();
		PL_PRINTF("[WinError %u] Error reading file handle '%lu'\n",err,file);
		return false;
	}
	void FileClose(APIFile* file){
		if(file)
			CloseHandle(TO_HANDLE(file));
	}
	bool FileExist(const std::string& path){
        DWORD attributes = GetFileAttributesA(path.c_str());   
        if(attributes == INVALID_FILE_ATTRIBUTES){
            DWORD err = GetLastError();
            PL_PRINTF("[WinError %u] GetFileAttributesA '%s'\n",err,path.c_str());
            return false;
        }
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }
	bool DirectoryCreate(const std::string& path){
		DWORD success = CreateDirectoryA(path.c_str(),0);
		if(!success){
			DWORD err = GetLastError();
			PL_PRINTF("[WinError %u] CreateDirectoryA '%s'\n",err,path.c_str());
            return false;
		}
		return true;
	}
    bool DirectoryExist(const std::string& path){
        DWORD attributes = GetFileAttributesA(path.c_str());   
        if(attributes == INVALID_FILE_ATTRIBUTES){
            DWORD err = GetLastError();
			if(err==ERROR_FILE_NOT_FOUND)
				return false;
            PL_PRINTF("[WinError %u] GetFileAttributesA '%s'\n",err,path.c_str());
            return false;
        }
        return (attributes & FILE_ATTRIBUTE_DIRECTORY);
    }
    bool FileLastWriteSeconds(const std::string& path, double* seconds){
        HANDLE handle = CreateFileA(path.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
		
		if(handle==INVALID_HANDLE_VALUE){
			DWORD err = GetLastError();
			if(err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND){
				PL_PRINTF("[WinError %u] Cannot find '%s'\n",err,path.c_str());
			}else if(err == ERROR_ACCESS_DENIED){
				PL_PRINTF("[WinError %u] Denied access to '%s'\n",err,path.c_str()); // tried to open a directory?
			}else {
				PL_PRINTF("[WinError %u] Error opening '%s'\n",err,path.c_str());
			}
			return false;
		}
        FILETIME time;
        BOOL success = GetFileTime(handle,0,0,&time);
        if(!success){
            DWORD err = GetLastError();
            PL_PRINTF("[WinError %u] GetFileTime '%s'\n",err,path.c_str());
            return false;
        }
        success = CloseHandle(handle);
        if(!success){
            DWORD err = GetLastError();
            PL_PRINTF("[WinError %u] CloseHandle '%s'\n",err,path.c_str());
        }
        uint64 somet = (uint64)time.dwLowDateTime+(uint64)time.dwHighDateTime*((uint64)MAXDWORD+1);
		*seconds = somet/10000000.f; // 100-nanosecond intervals
        return true;
    }
    
    // static std::mutex s_allocStatsMutex;
	static uint64 s_totalAllocatedBytes=0;
	static uint64 s_totalNumberAllocations=0;
	static uint64 s_allocatedBytes=0;
	static uint64 s_numberAllocations=0;
	void* Allocate(uint64 bytes){
		if(bytes==0) return nullptr;
		void* ptr = malloc(bytes);
		if(!ptr) return nullptr;
		
		// s_allocStatsMutex.lock();
		s_allocatedBytes+=bytes;
		s_numberAllocations++;
		s_totalAllocatedBytes+=bytes;
		s_totalNumberAllocations++;			
		// s_allocStatsMutex.unlock();
		
		return ptr;
	}
    void* Reallocate(void* ptr, uint64 oldBytes, uint64 newBytes){
        if(newBytes==0){
            Free(ptr,oldBytes);
            return nullptr;   
        }else{
            if(ptr==0){
                return Allocate(newBytes);   
            }else{
                if(oldBytes==0){
                    PL_PRINTF("Reallocate : oldBytes is zero while the ptr isn't!?\n");   
                }
                void* newPtr = realloc(ptr,newBytes);
                if(!newPtr)
                    return nullptr;
                
                // s_allocStatsMutex.lock();
                s_allocatedBytes+=newBytes-oldBytes;
                
                s_totalAllocatedBytes+=newBytes;
                s_totalNumberAllocations++;			
                // s_allocStatsMutex.unlock();
                return newPtr;
            }
        }
    }
	void Free(void* ptr, uint64 bytes){
		if(!ptr) return;
		free(ptr);
		
		// s_allocStatsMutex.lock();
		s_allocatedBytes-=bytes;
		s_numberAllocations--;
		// s_allocStatsMutex.unlock();
	}
	uint64 GetTotalAllocatedBytes(){
		return s_totalAllocatedBytes;
	}
	uint64 GetTotalNumberAllocations(){
		return s_totalNumberAllocations;
	}
	uint64 GetAllocatedBytes(){
		return s_allocatedBytes;
	}
	uint64 GetNumberAllocations(){
		return s_numberAllocations;
	}
	static HANDLE m_consoleHandle = NULL;
    void SetConsoleColor(uint16 color){
		if (m_consoleHandle == NULL) {
			m_consoleHandle = GetStdHandle(-11);
			if (m_consoleHandle == NULL)
				return;
		}
		// Todo: don't set color if already set? difficult if you have a variable of last color and a different 
		//		function sets color without changing the variable.
		SetConsoleTextAttribute((HANDLE)m_consoleHandle, color);
	}
    Semaphore::Semaphore(int initial, int max) {
		m_initial = initial;
		m_max = max;
	}
	Semaphore::~Semaphore() {
		cleanup();
	}
	void Semaphore::cleanup() {
		if (m_internalHandle != 0){
            BOOL yes = CloseHandle(TO_HANDLE(m_internalHandle));
            if(!yes){
                DWORD err = GetLastError();
                PL_PRINTF("[WinError %u] CloseHandle\n",err);
            }
			m_internalHandle=0;
        }
	}
	void Semaphore::wait() {
		if (m_internalHandle == 0) {
			HANDLE handle = CreateSemaphore(NULL, m_initial, m_max, NULL);
			if (handle == INVALID_HANDLE_VALUE) {
				DWORD err = GetLastError();
                PL_PRINTF("[WinError %u] CreateSemaphore\n",err);
			}else
                m_internalHandle = TO_INTERNAL(handle);
		}
		if (m_internalHandle != 0) {
			DWORD res = WaitForSingleObject(TO_HANDLE(m_internalHandle), INFINITE);
			if (res == WAIT_FAILED) {
				DWORD err = GetLastError();
				PL_PRINTF("[WinError %u] WaitForSingleObject\n",err);
			}
		}
	}
	void Semaphore::signal(int count) {
		if (m_internalHandle != 0) {
			BOOL yes = ReleaseSemaphore(TO_HANDLE(m_internalHandle), count, NULL);
			if (!yes) {
				DWORD err = GetLastError();
				PL_PRINTF("[WinError %u] ReleaseSemaphore\n",err);
			}
		}
	}
	Mutex::~Mutex() {
		cleanup();
	}
	void Mutex::cleanup() {
        if (m_internalHandle != 0){
            BOOL yes = CloseHandle(TO_HANDLE(m_internalHandle));
            if(!yes){
                DWORD err = GetLastError();
                PL_PRINTF("[WinError %u] CloseHandle\n",err);
            }
			m_internalHandle=0;
        }
	}
	void Mutex::lock() {
		if (m_internalHandle == 0) {
			HANDLE handle = CreateMutex(NULL, false, NULL);
			if (handle == INVALID_HANDLE_VALUE) {
				BOOL err = GetLastError();
				PL_PRINTF("[WinError %u] CreateMutex\n",err);
			}else
                m_internalHandle = TO_INTERNAL(handle);
		}
		if (m_internalHandle != 0) {
			DWORD res = WaitForSingleObject(TO_HANDLE(m_internalHandle), INFINITE);
			uint32 newId = Thread::GetThisThreadId();
			if (m_ownerThread != 0) {
				PL_PRINTF("Mutex : Locking twice, old owner: %u, new owner: %u\n",m_ownerThread,newId);
			}
			m_ownerThread = newId;
			if (res == WAIT_FAILED) {
                // Todo: What happened do the old thread who locked the mutex. Was it okay to ownerThread = newId
				DWORD err = GetLastError();
                PL_PRINTF("[WinError %u] WaitForSingleObject\n",err);
			}
		}
	}
	void Mutex::unlock() {
		if (m_internalHandle != 0) {
			m_ownerThread = 0;
			BOOL yes = ReleaseMutex(TO_HANDLE(m_internalHandle));
			if (!yes) {
				DWORD err = GetLastError();
                PL_PRINTF("[WinError %u] ReleaseMutex\n",err);
			}
		}
	}
	uint32_t Mutex::getOwner() {
		return m_ownerThread;
	}
    Thread::~Thread() {
		cleanup();
	}
	void Thread::cleanup() {
		if (m_internalHandle) {
			if (m_threadId == GetThisThreadId()) {
                PL_PRINTF("Thread : Thread cannot clean itself\n");
				return;
			}
            BOOL yes = CloseHandle(TO_HANDLE(m_internalHandle));
            if(!yes){
                DWORD err = GetLastError();
                PL_PRINTF("[WinError %u] CloseHandle\n",err);
            }
			m_internalHandle=0;
		}
	}
	void Thread::init(uint32(*func)(void*), void* arg) {
		if (!m_internalHandle) {
			// const uint32 stackSize = 1024*1024;
			HANDLE handle = CreateThread(NULL, 0, (DWORD(*)(void*))func, arg, 0,(DWORD*)&m_threadId);
			if (handle==INVALID_HANDLE_VALUE) {
				DWORD err = GetLastError();
                PL_PRINTF("[WinError %u] CreateThread\n",err);
			}else
                m_internalHandle = TO_INTERNAL(handle);
		}
	}
	void Thread::join() {
		if (!m_internalHandle)
			return;
		DWORD res = WaitForSingleObject(TO_HANDLE(m_internalHandle), INFINITE);
		if (res==WAIT_FAILED) {
			DWORD err = GetLastError();
            PL_PRINTF("[WinError %u] WaitForSingleObject\n",err);
		}
		BOOL yes = CloseHandle(TO_HANDLE(m_internalHandle));
        if(!yes){
            DWORD err = GetLastError();
            PL_PRINTF("[WinError %u] CloseHandle\n",err);
        }
		m_internalHandle = 0;
	}
	bool Thread::isActive() {
		return m_internalHandle!=0;
	}
	bool Thread::joinable() {
		return m_threadId != GetCurrentThreadId();
	}
	ThreadId Thread::getId() {
		return m_threadId;
	}
	ThreadId Thread::GetThisThreadId() {
		return GetCurrentThreadId();
	}
}
#endif