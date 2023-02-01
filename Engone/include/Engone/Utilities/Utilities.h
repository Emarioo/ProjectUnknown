#pragma once

#include "Engone/Logger.h"
#include "Engone/LoopInfo.h"
#include "Engone/Utilities/Tracker.h"

#include "Engone//Utilities/FileUtility.h"
#include "Engone//Utilities/TimeUtility.h"
#include "Engone//Utilities/RandomUtility.h"
#include "Engone//Utilities/ImageUtility.h"
#include "Engone/Utilities/Alloc.h"

namespace engone {

	std::vector<std::string> SplitString(std::string text, std::string delim);
	//std::string SanitizeString(std::string s);
	
	float lerp(float a, float b, float c);
	float inverseLerp(float min, float max, float x);
	float distance(float x, float y, float x1, float y1);
	//void insert(float* ar, int off, int len, float* data);
	float bezier(float x, float xStart, float xEnd);
	//std::string Crypt(const std::string& word,const std::string& key, bool encrypt);
	
	// differnce between two angles, if difference is large clockwise then difference will be negative.
	float AngleDifference(float a, float b);
	//void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3);

	void ConvertWide(const std::string& in, std::wstring& out);
	void ConvertWide(const std::wstring& in, std::string& out);

	// A vector but you can have differently sized classes and structs.
	class ItemVector {
	public:
		ItemVector(int size = 0);
		~ItemVector();

		template<class T>
		void writeMemory(char type, void* ptr) {
			int itemSize = sizeof(T);
			if (m_maxSize < m_writeIndex + sizeof(char) + itemSize) {
				if (!resize((m_maxSize + sizeof(char) + itemSize) * 2)) {
					return;
				}
			}

			*(m_data + m_writeIndex) = type;
			m_writeIndex += sizeof(char);
			memcpy_s(m_data + m_writeIndex, m_maxSize - m_writeIndex, ptr, itemSize);
			m_writeIndex += itemSize;
		}
		// Returns 0 if end is reached
		char readType();
		template<class T>
		T* readItem() {
			if (m_writeIndex < m_readIndex + sizeof(T)) {
				log::out << "reached end of HeapMemory\n";
				return nullptr;
			}

			char* ptr = m_data + m_readIndex;
			m_readIndex += sizeof(T);
			return (T*)ptr;
		}
		// true if there are NO items
		bool empty() const;
		// reset read and write index.
		void clear();
		// will reset the class making it as good as new.
		void cleanup();

		static TrackerId trackerId;
	private:
		char* m_data = nullptr;
		uint32_t m_maxSize = 0;
		uint32_t m_writeIndex = 0;
		uint32_t m_readIndex = 0;

		bool resize(uint32_t size);
	};

	// start will activate the class, run should be inside update loop which will update
	// the class's inner timer. Once reached, run will return true once otherwise false.
	// use start again to restart.
	class DelayCode {
	public:
		DelayCode() = default;
		DelayCode(float waitTime) : running(true), waitTime(waitTime) {}

		void start(float waitInSeconds) { waitTime = waitInSeconds; time = 0; running = true; }
		void stop() { running = false; }
		// info is here incase you want to slow down the entire app.
		// Without info, this class would not be affected.
		bool run(float deltaTime) {
			if (running) {
				time += deltaTime;
				if (time >= waitTime) {
					running = false;
					return true;
				}
			}
			return false;
		}
		bool run(LoopInfo& info) {
			return run(info.timeStep);
		}

		// global methods
		static void Start(int id, float waitSeconds);
		static void Stop(int id, float waitSeconds);
		static bool Run(int id, float deltaTime);
		static bool Run(int id, LoopInfo& info);

	private:
		bool running = false;
		float time = 0;
		float waitTime = 0;
	};

	// Value template requires <, >, == operators
	//template<typename Value>
	// BinaryTree is not finished, just copied from tracker. needs testing
	class BinaryTree;
	class BinaryNode {
	public:
		typedef int Value;

		BinaryNode() = default;

		// returns true if added, false if ptr already exists, or if something failed
		bool add(BinaryTree* tree, Value ptr);

		bool find(BinaryTree* tree, Value value);

		// the returned node was detached when you broke a node. The other node replaced the removed node.
		// returned node is nullptr if no node was detached
		bool remove(BinaryTree* tree, uint32_t& ref, Value ptr);
		void replace(BinaryTree* tree, Value ptr, uint32_t index);
		void reattach(BinaryTree* tree, uint32_t index);

		Value m_value;
		uint32_t left;
		uint32_t right;
	};
	class BinaryTree {
	public:
		typedef int Value;
		BinaryTree() = default;
		~BinaryTree() { cleanup(); }
		void cleanup();

		// returns true if added, false if ptr already exists
		bool add(Value value);
		// returns true if ptr was removed, false if it didn't exist
		bool remove(Value value);
		bool find(Value value);
		//void print() const;

		// free memory

		//void printNodes() const;

	private:
		uint32_t rootIndex = 0;
		uint32_t capacity = 0; // in count
		uint32_t head = 0; // int count
		// only change the allocation with resize
		BinaryNode* data = nullptr;

		BinaryNode* getNode(uint32_t index) const;
		uint32_t newNode();
		BinaryNode* getRoot() const;

		std::mutex m_mutex;

		// size is the count of nodes.
		bool resize(uint32_t size);

		friend class BinaryNode;

	};

	//void CountingTest(int times, int numElements, std::function<int()> func);

	// Starts an exe at path. Uses CreateProcess from windows.h
	// commandLine cannot be constant (CreateProcessA in windows api says so)
	bool StartProgram(const std::string& path, char* commandLine=NULL);

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
		enum Flags : uint32 {
			WATCH_SUBTREE = 1,
		};
		enum ChangeType : uint32 {
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
		bool check(const std::string& root, std::function<void(const std::string&, uint32)> callback, uint32 flags = 0);

		inline const std::string& getRoot() { return m_root; }

		inline std::function<void(const std::string&, uint32)>& getCallback() { return m_callback; }

	private:
		bool m_running = false;
		std::function<void(const std::string&, uint32)> m_callback;
		std::string m_root; // path passed to check function
		std::string m_dirPath; // if m_root isn't a directory then this will be the dir of the file
		uint32 m_flags = 0;

		HANDLE m_changeHandle=NULL;
		std::mutex m_mutex;
		std::thread m_thread;
		//HANDLE m_threadHandle = NULL; // used to cancel ReadDirectoryChangesW
	
		HANDLE m_dirHandle = NULL;
		void* m_buffer=nullptr;
		uint32 m_bufferSize=0;
		static const uint32 INITIAL_SIZE = 5 * (sizeof(FILE_NOTIFY_INFORMATION) + 500);
	};
	
	// You set a threads name with it. Use -1 as threadId if you want to set the name for the current thread.
	// This only works in visual studios debugger.
	// An alternative is SetThreadDescription but it is not available in Windows 8.1 which I am using.
	void SetThreadName(DWORD dwThreadID, const char* threadName);
}