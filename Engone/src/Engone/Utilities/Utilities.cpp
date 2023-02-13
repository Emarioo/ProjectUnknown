
#include "Engone/Utilities/Utilities.h"

#include "Engone/vendor/stb_image/stb_image.h"

#include "Engone/Win32Includes.h"
#include <thread>

namespace engone {
	// TrackerId ItemVector::trackerId = "ItemVector";

	std::vector<std::string> SplitString(std::string text, std::string delim) {
		std::vector<std::string> out;

		int lastAt = 0;
		while (true) {
			int at = text.find(delim, lastAt);
			if (at == -1) {
				break;
			}
			std::string push = text.substr(lastAt, at - lastAt);
			out.push_back(push);
			lastAt = at + 1;
		}
		if (lastAt != text.size() || lastAt == 0)
			out.push_back(text.substr(lastAt));


		//unsigned int at = 0;
		//while ((at = text.find(delim)) != std::string::npos) {
		//	std::string push = text.substr(0, at);
		//	//if (push.size() > 0) {
		//	out.push_back(push);
		//	//}
		//	text.erase(0, at + delim.length());
		//}
		//if (text.size() != 0)
		//	out.push_back(text);
		return out;
	}

	/*
	void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3) {
		ar[ind] = f0;
		ar[ind + 1] = f1;
		ar[ind + 2] = f2;
		ar[ind + 3] = f3;
	}*/
	float lerp(float a, float b, float c) {
		return (1 - c) * a + c * b;
	}
	float inverseLerp(float min, float max, float x) {
		return (x - min) / (max - min);
	}
	float distance(float x, float y, float x1, float y1) {
		return (float)sqrt(pow(x1 - x, 2) + pow(y1 - y, 2));
	}
	float AngleDifference(float a, float b) {
		float d = a - b;
		if (d > glm::pi<float>()) {
			d -= glm::pi<float>() * 2;
		}
		if (d < -glm::pi<float>()) {
			d += glm::pi<float>() * 2;
		}
		return d;
	}
	/*
	void insert(float* ar, int off, int len, float* data) { // carefull with going overboard
		//std::cout << "INSERT ";
		for (int i = 0; i < len; i++) {
			ar[off + i] = data[i];
			//std::cout << off + i << "_" << ar[off + i] << "   ";
		}
		//std::cout << std::endl;
	}*/
	// returned value goes from 0 to 1. x goes from xStart to xEnd. Function does not handle errors like 1/0
	float bezier(float x, float xStart, float xEnd) {
		float t = (x - xStart) / (xEnd - xStart);
		float va = /*(pow(1 - t, 3) + 3 * pow(1 - t, 2) * t) +*/(3 * (1 - t) * (float)pow(t, 2) + (float)pow(t, 3));
		return va;
	}
	float Min(float a, float b) {
		if (a < b) return a;
		return b;
	}
	float Max(float a, float b) {
		if (a > b) return a;
		return b;
	}
	/*
	std::string Crypt(const std::string& word, const std::string& key, bool encrypt) {
		std::string out = "";
		for (int i = 0; i < word.length();i++) {
			int at = word[i];
			for (int j = 0; j < key.length();j++) {
				int val = (i - j) * key[j];
				if (encrypt) at += val;
				else at -= val;
			}
			out += (at % 256);
		}
		return out;
	}*/

	ItemVector::ItemVector(int size) {
		if (size != 0) {
			resize(size); // may fail but it's fine
		}
	}
	ItemVector::~ItemVector() {
		cleanup();
	}
	char ItemVector::readType() {
		if (m_writeIndex < m_readIndex + sizeof(char)) {
			return 0;
		}

		char type = *(m_data + m_readIndex);
		m_readIndex += sizeof(char);
		return type;
	}
	bool ItemVector::empty() const {
		return m_writeIndex == 0;
	}
	void ItemVector::clear() {
		m_writeIndex = 0;
		m_readIndex = 0;
	}
	void ItemVector::cleanup() {
		resize(0); // <- this will also set write and read to 0
	}
	bool ItemVector::resize(uint32_t size) {
		if (size == 0) {
			if (m_data) {
				alloc::free(m_data, m_maxSize);
				// GetTracker().subMemory<ItemVector>(m_maxSize);
				m_data = nullptr;
				m_maxSize = 0;
				m_writeIndex = 0;
				m_readIndex = 0;
			}
		} else {
			if (!m_data) {
				char* newData = (char*)alloc::malloc(size);
				if (!newData) {
					log::out << log::RED << "ItemVector failed allocation memory\n";
					return false;
				}
				// GetTracker().addMemory<ItemVector>(size);
				m_data = newData;
				m_maxSize = size;
			} else {
				char* newData = (char*)alloc::realloc(m_data, m_maxSize, size);
				if (!newData) {
					log::out << log::RED << "ItemVector failed reallocating memory\n";
					return false;
				}
				// GetTracker().subMemory<ItemVector>(m_maxSize);
				// GetTracker().addMemory<ItemVector>(size);
				m_data = newData;
				m_maxSize = size;
				if (m_writeIndex > m_maxSize)
					m_writeIndex = m_maxSize;
				if (m_readIndex > m_maxSize)
					m_readIndex = m_maxSize;
			}
		}
		return true;
	}
	/*
	void CountingTest(int times, int numElements, std::function<int()> func) {
		std::vector<int> occurrences(numElements);

		double bef = GetSystemTime();
		for (int i = 0; i < times; i++) {
			int index = func();
			if(index>-1&&index<numElements)
				occurrences[func()]++;
		}
		double aft = GetSystemTime();
		for (int i = 0; i < numElements; i++) {
			std::cout << i << " " << occurrences[i] << std::endl;
		}
		std::cout << "Time: " << (aft - bef) << std::endl;
	}*/

	static std::unordered_map<int, DelayCode> delayers;
	void DelayCode::Start(int id, float waitSeconds) {
		delayers[id] = { waitSeconds };
	}
	void DelayCode::Stop(int id, float waitSeconds) {
		auto find = delayers.find(id);
		DelayCode* dc = nullptr;
		if (find != delayers.end()) {
			find->second.stop();
		}
	}
	bool DelayCode::Run(int id, float deltaTime) {
		auto find = delayers.find(id);
		DelayCode* dc = nullptr;
		if (find != delayers.end()) {
			return find->second.run(deltaTime);
		}
		return false;
	}
	bool DelayCode::Run(int id, LoopInfo& info) {
		return Run(id, info.timeStep);
	}

	//Timer::Timer() : time(GetAppTime()) { }
	//Timer::Timer(const std::string& str) : time(GetAppTime()), name(str) { }
	//Timer::Timer(const std::string& str, int id) : time(GetAppTime()), name(str), id(id) { }
	//Timer::~Timer() {
	//	if (time != 0) end();
	//}
	//void Timer::end() {
	//	if (id != 0) {
	//		if (timerCounting.count(id)>0) {
	//			timerCounting[id]++;
	//			if (timerCounting[id] < 60) {
	//				return;
	//			}else{
	//				timerCounting[id] = 0;
	//			}
	//		} else {
	//			timerCounting[id] = 0;
	//			return;
	//		}
	//	}

	//	log::out << name << " : " << (GetAppTime() - time) << "\n";
	//	time = 0;
	//}
	void ConvertWide(const std::string& in, std::wstring& out) {
		out.resize(in.length(), 0);
		for (int i = 0; i < in.length(); i++) {
			out.data()[i] = in[i];
		}
	}
	void ConvertWide(const std::wstring& in, std::string& out) {
		out.resize(in.length(), 0);
		for (int i = 0; i < in.length(); i++) {
			out.data()[i] = in[i];
		}
	}
	bool StartProgram(const std::string& path, char* commandLine) {
		if (!FileExist(path)) {
			return false;
		}

		// additional information
		STARTUPINFOA si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		int slashIndex = path.find_last_of("\\");

		std::string workingDir = path.substr(0, slashIndex);

		//#ifdef NDEBUG
		//		std::wstring exeFile = convert(path);
		//		std::wstring workDir = convert(workingDir);
		//#else
		const std::string& exeFile = path;
		std::string& workDir = workingDir;
		//#endif
		CreateProcessA(exeFile.c_str(),   // the path
			commandLine,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			workDir.c_str(),   // starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		);

		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}

	void BinaryTree::cleanup() {
		m_mutex.lock();
		resize(0);
		m_mutex.unlock();
	}
	bool BinaryTree::add(Value ptr) {
		m_mutex.lock();
		bool quit = false;
		// make sure i can add another node if i need to.
		if (capacity < head + 1) {
			if (!data) {
				if (!resize(5)) {
					quit = true;
				}
			} else {
				if (!resize((head + 1) * 2)) {
					quit = true;
				}
			}
		}
		bool out = false;
		if (quit) {
			if (getRoot()) {
				out = getRoot()->add(this, ptr);
			} else {
				rootIndex = newNode();
				if (getRoot()) {
					memset(getRoot(), 0, sizeof(BinaryNode));
					getRoot()->m_value = ptr;
					out = true;
				} else
					out = false;
			}
		}
		m_mutex.unlock();
		return out;
	}
	// returns true if ptr was removed, false if it didn't exist
	bool BinaryTree::remove(Value ptr) {
		m_mutex.lock();
		bool out = false;
		if (getRoot())
			out = getRoot()->remove(this, rootIndex, ptr);
		m_mutex.unlock();
		return out;
	}
	bool BinaryTree::find(Value value) {
		m_mutex.lock();
		bool out = false;
		if (getRoot())
			out = getRoot()->find(this, value);
		m_mutex.unlock();
		return out;
	}
	BinaryNode* BinaryTree::getNode(uint32_t index) const {
		if (index == 0) return nullptr;
		return data + index - 1;
	}
	uint32_t BinaryTree::newNode() {
		if (capacity < head + 1) {
			return 0; // failed, resizing here is dangerous
		}
		memset(data + head, 0, sizeof(BinaryNode));
		head++;
		return head;
	}
	BinaryNode* BinaryTree::getRoot() const {
		return getNode(rootIndex);
	}
	// size is the count of nodes.
	bool BinaryTree::resize(uint32_t size) {
		if (size == 0) {
			if (data) {
				alloc::free(data, sizeof(BinaryNode) * capacity);
				//GetTracker().subMemory<BinaryNode>(sizeof(BinaryNode) * capacity);
				data = nullptr;
				capacity = 0;
				head = 0;
				rootIndex = 0;
			}
		} else {
			if (!data) {
				BinaryNode* newData = (BinaryNode*)alloc::malloc(sizeof(BinaryNode) * size);
				if (!newData) return false;
				//GetTracker().addMemory<TrackNode>(sizeof(TrackNode) * size);
				data = newData;
				capacity = size;
			} else {
				BinaryNode* newData = (BinaryNode*)alloc::realloc(data, capacity * sizeof(BinaryNode), sizeof(BinaryNode) * size);
				if (!newData) return false;
				//GetTracker().subMemory<TrackNode>(sizeof(TrackNode) * (maxCount));
				//GetTracker().addMemory<TrackNode>(sizeof(TrackNode) * (size));
				data = newData;
				capacity = size;
				if (head > capacity) head = capacity;
			}
		}
		return true;
	}
	// returns true if added, false if ptr already exists, or if something failed
	bool BinaryNode::add(BinaryTree* tree, Value ptr) {
		BinaryNode* a = tree->getNode(left);
		BinaryNode* b = tree->getNode(right);
		if (m_value == ptr) return false; // could not add ptr
		if (ptr < m_value) {
			if (a) {
				return a->add(tree, ptr);
			} else {
				left = tree->newNode();
				a = tree->getNode(left);
				if (a) {
					a->m_value = ptr;
					return true;
				} else return false;
			}
		} else {
			if (b) {
				return b->add(tree, ptr);
			} else {
				right = tree->newNode();
				b = tree->getNode(right);
				if (b) {
					b->m_value = ptr;
					return true;
				} else return false;
			}
		}
	}
	bool BinaryNode::find(BinaryTree* tree, Value ptr) {
		BinaryNode* a = tree->getNode(left);
		BinaryNode* b = tree->getNode(right);
		if (m_value == ptr) return false; // could not add ptr
		if (ptr < m_value) {
			if (a) {
				return a->find(tree, ptr);
			} else {
				return false;
			}
		} else {
			if (b) {
				return b->find(tree, ptr);
			} else {
				return false;
			}
		}
	}
	// the returned node was detached when you broke a node. The other node replaced the removed node.
	// returned node is nullptr if no node was detached
	bool BinaryNode::remove(BinaryTree* tree, uint32_t& ref, Value ptr) {
		BinaryNode* a = tree->getNode(left);
		BinaryNode* b = tree->getNode(right);
		if (m_value == ptr) {
			uint32_t loose1 = 0;
			uint32_t loose2 = 0;
			// ALL THIS SHOULD NOT FAIL IF THERE ARE NO LOOSE NODES

			BinaryNode* cpy = tree->getNode(tree->head);// head-1 is the actual location, getNode decrements index by one. so it adds up.

			// could be nullptr because ref = loose1
			// find the node that refers to the last node and change it to refer to this
			BinaryNode* root = tree->getRoot();
			if (root) {
				if (root->m_value == cpy->m_value) tree->rootIndex = ref;
				else root->replace(tree, cpy->m_value, ref);
			}
			if (ref % 2 == 0) { // for better distribution
				if (a) {
					loose1 = left;
					loose2 = right;
				} else if (b) {
					loose1 = right;
				}
			} else {
				if (b) {
					loose1 = right;
					loose2 = left;
				} else if (a) {
					loose1 = left;
				}
			}

			uint32_t oldRef = ref; // for debugging

			// attach a loose node to the parent
			ref = loose1;

			//*this = *(tree->data + tree->head - 1);
			// move last node to memory at this

			// remove last node
			tree->head--;

			BinaryNode old = *this; // copy for debugging

			m_value = cpy->m_value;
			left = cpy->left;
			right = cpy->right;

			if (loose2) // no need to attach nullptr
				tree->getRoot()->reattach(tree, loose2); // reattach to parent node is enough but i don't have access to it so root will do.

			return true;
		} else if (ptr < m_value) {
			if (a)
				return a->remove(tree, left, ptr);
		} else if (ptr > m_value) {
			if (b)
				return b->remove(tree, right, ptr);
		}
		return false;
	}
	void BinaryNode::replace(BinaryTree* tree, Value ptr, uint32_t index) {
		BinaryNode* a = tree->getNode(left);
		BinaryNode* b = tree->getNode(right);
		if (ptr < m_value) {
			if (a) {
				if (a->m_value == ptr) {
					left = index;
				} else {
					a->replace(tree, ptr, index);
				}
			}
		} else if (ptr > m_value) {
			if (b) {
				if (b->m_value == ptr) {
					right = index;
				} else {
					b->replace(tree, ptr, index);
				}
			}
		} else {
			// do nothing, replacing yourself doesn't work
			log::out << log::RED << "replaced self\n"; // hasn't happened, hopefully won't
		}
	}
	void BinaryNode::reattach(BinaryTree* tree, uint32_t index) {
		BinaryNode* a = tree->getNode(left);
		BinaryNode* b = tree->getNode(right);
		BinaryNode* node = tree->getNode(index);
		if (node->m_value < m_value) {
			if (a) {
				a->reattach(tree, index);
			} else {
				left = index;
			}
		} else if (node->m_value > m_value) {
			if (b) {
				b->reattach(tree, index);
			} else {
				right = index;
			}
		} else {
			//tree->print();
			log::out << log::RED << "Reattach, found an equal ptr, should not happen!\n";
		}
	}
	void ConvertArguments(int& argc, char**& argv) {
		LPWSTR wstr = GetCommandLineW();

		wchar_t** wargv = CommandLineToArgvW(wstr, &argc);

		if (wargv == NULL) {
			int err = GetLastError();
			log::out << log::RED << "ConvertArguments - " << err << "\n";
		} else {
			// argv will become a pointer to contigous memory which contain arguments.
			int totalSize = argc * sizeof(char*);
			int index = totalSize;
			for (int i = 0; i < argc; i++) {
				int length = wcslen(wargv[i]);
				//printf("len: %d\n", length);
				totalSize += length + 1;
			}
			//printf("size: %d index: %d\n", totalSize,index);
			argv = (char**)alloc::malloc(totalSize);
			char* argData = (char*)argv + index;
			if (!argv) {
				log::out << log::RED << "ConverArguments - allocation failed\n";
			} else {
				index = 0;
				for (int i = 0; i < argc; i++) {
					int length = wcslen(wargv[i]);

					argv[i] = argData + index;

					for (int j = 0; j < length; j++) {
						argData[index] = wargv[i][j];
						index++;
					}
					argData[index++] = 0;
				}
			}
		}
		LocalFree(wargv);
	}
	void ConvertArguments(const char* args, int& argc, char**& argv) {
		if (args == nullptr) {
			log::out << log::RED << "ConvertArguments - args was null\n";
		} else {
			argc = 0;
			// argv will become a pointer to contigous memory which contain arguments.
			int dataSize = 0;
			int argsLength = strlen(args);
			int argLength = 0;
			for (int i = 0; i < argsLength + 1; i++) {
				char chr = args[i];
				if (chr == 0 || chr == ' ') {
					if (argLength != 0) {
						dataSize++; // null terminated character
						argc++;
					}
					argLength = 0;
					if (chr == 0)
						break;
				} else {
					argLength++;
					dataSize++;
				}
			}
			int index = argc * sizeof(char*);
			int totalSize = index + dataSize;
			//printf("size: %d index: %d\n", totalSize,index);
			argv = (char**)alloc::malloc(totalSize);
			char* argData = (char*)argv + index;
			if (!argv) {
				log::out << log::RED << "ConverArguments - allocation failed\n";
			} else {
				int strIndex = 0; // index of char*
				for (int i = 0; i < argsLength + 1; i++) {
					char chr = args[i];

					if (chr == 0 || chr == ' ') {
						if (argLength != 0) {
							argData[i] = 0;
							dataSize++; // null terminated character
						}
						argLength = 0;
						if (chr == 0)
							break;
					} else {
						if (argLength == 0) {
							argv[strIndex] = argData + i;
							strIndex++;
						}
						argData[i] = chr;
						argLength++;
					}
				}
			}
		}
	}
	void FreeArguments(int argc, char** argv) {
		int totalSize = argc * sizeof(char*);
		int index = totalSize;
		for (int i = 0; i < argc; i++) {
			int length = strlen(argv[i]);
			//printf("len: %d\n", length);
			totalSize += length + 1;
		}
		alloc::free(argv, totalSize);
	}
	void CreateConsole() {
		bool b = AllocConsole();
		if (b) {
			freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
			freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
		}
	}
	FileMonitor::~FileMonitor() { cleanup(); }
	void FileMonitor::cleanup() {
		m_mutex.lock();
		m_running = false;
		if (m_thread.joinable()) {
			HANDLE handle = m_thread.m_internalHandle;
			int err = CancelSynchronousIo(handle);
			if (err == 0) {
				err = GetLastError();
				// ERROR_NOT_FOUND
				if (err != ERROR_NOT_FOUND)
					log::out << log::RED << "FileMonitor::cleanup - err " << err << "\n";
			}
		}
		if (m_changeHandle != NULL) {
			FindCloseChangeNotification(m_changeHandle);
			m_changeHandle = NULL;
		}
		if (m_dirHandle != NULL) {
			CloseHandle(m_dirHandle);
			m_dirHandle = NULL;
		}
		m_mutex.unlock();

		if (m_thread.joinable())
			m_thread.join();
		//m_threadHandle = NULL;
		if (m_buffer) {
			alloc::free(m_buffer, m_bufferSize);
			m_buffer = nullptr;
		}
		// m_running is set to false in thread
	}
	uint32 RunMonitor(void* arg){
		FileMonitor* self = (FileMonitor*)arg;
		SetThreadName(GetCurrentThreadId(), "FileMonitor");
		//m_threadHandle = GetCurrentThread();
		std::string temp;
		DWORD waitStatus;
		while (true) {
			waitStatus = WaitForSingleObject(self->m_changeHandle, INFINITE);

			if (!self->m_running)
				break;
			if (waitStatus == WAIT_OBJECT_0) {
				//log::out << "FileMonitor::check - catched " << m_root << "\n";

				if (!self->m_buffer) {
					self->m_buffer = (FILE_NOTIFY_INFORMATION*)alloc::malloc(FileMonitor::INITIAL_SIZE);
					if (!self->m_buffer) {
						self->m_bufferSize = 0;
						log::out << log::RED << "FileMonitor::check - buffer allocation failed\n";
						break;
					}
					self->m_bufferSize = FileMonitor::INITIAL_SIZE;
				}

				DWORD bytes = 0;
				BOOL success = ReadDirectoryChangesW(self->m_dirHandle, self->m_buffer, self->m_bufferSize, self->m_flags & FileMonitor::WATCH_SUBTREE, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME, &bytes, NULL, NULL);
				//log::out << "ReadDir change\n";
				if (bytes == 0) {
					// try to read changes again but with bigger buffer? while loop?
					//log::out << log::RED << "FileMonitor::check - buffer to small or big\n";
					// this could also mean that we cancelled the read.
				}
				if (success == 0) {
					int err = GetLastError();

					// ERROR_INVALID_HANDLE
					// ERROR_INVALID_PARAMETER
					// ERROR_INVALID_FUNCTION
					// ERROR_OPERATION_ABORTED

					if (err != ERROR_OPERATION_ABORTED)
						// this could also mean that we cancelled the read.
						log::out << log::RED << "FileMonitor::check - ReadDirectoryChanges win error " << err << " (path: " << self->m_root << ")\n";
					break;
				} else {
					int offset = 0;
					while (true) {
						FILE_NOTIFY_INFORMATION& info = *(FILE_NOTIFY_INFORMATION*)((char*)self->m_buffer + offset);
						int length = info.FileNameLength / sizeof(WCHAR);
						if (length < MAX_PATH + 1) {
							temp.resize(length);
							for (int i = 0; i < length; i++) {
								temp.data()[i] = (char)*(info.FileName + i);
							}
						}

						if (self->m_dirPath == self->m_root || temp == self->m_root) {
							//log::out << "FileMonitor::check - call callback " << temp << "\n";
							FileMonitor::ChangeType type = (FileMonitor::ChangeType)0;
							if (info.Action == FILE_ACTION_MODIFIED || info.Action == FILE_ACTION_ADDED) type = FileMonitor::FILE_MODIFIED;
							if (info.Action == FILE_ACTION_REMOVED) type = FileMonitor::FILE_REMOVED;
							if (type == 0) {
								log::out << log::YELLOW << "FileMonitor - type was 0 (info.Action=" << (int)info.Action << ")\n";
								//DebugBreak();
							}
							self->m_callback(temp, type);
						}

						if (info.NextEntryOffset == 0)
							break;
						offset += info.NextEntryOffset;
					}
				}

				self->m_mutex.lock();
				bool yes = FindNextChangeNotification(self->m_changeHandle);
				self->m_mutex.unlock();

				if (!yes) {
					// handle could have been closed
					break;
				}
			} else {
				// not sure why we are here but it isn't good so stop.
				break;
			}
		}
		self->m_mutex.lock();
		self->m_running = false;

		if (self->m_changeHandle) {
			FindCloseChangeNotification(self->m_changeHandle);
			self->m_changeHandle = NULL;
		}

		if (self->m_dirHandle) {
			CloseHandle(self->m_dirHandle);
			self->m_dirHandle = NULL;
		}
		self->m_root.clear();
		self->m_mutex.unlock();
		//log::out << "FileMonitor - finished thread\n";
		return 0;
	}
	// Todo: handle is checked against NULL, it should be checked against INVALID_HANDLE_VALUE
	bool FileMonitor::check(const std::string& path, std::function<void(const std::string&, uint32)> callback, uint32 flags) {
		if (!std::filesystem::exists(path))
			return false;
		//log::out << log::RED << "FileMonitor::check - invalid path : " << m_root << "\n";

		m_mutex.lock();

		// Just a heads up for you. This functions is not beautiful.

		if (m_root == path) { // no reason to check the same right?
			m_mutex.unlock();
			return true;
		}
		//if (m_threadHandle) {
		//	CancelSynchronousIo(m_threadHandle);
		//}
		if (m_changeHandle != NULL) {
			FindCloseChangeNotification(m_changeHandle);
			m_changeHandle = NULL;
		}
		if (m_dirHandle) {
			CloseHandle(m_dirHandle);
			m_dirHandle = NULL;
		}

		if (!m_running) {
			if (m_thread.isActive()) // Todo: What if
				m_thread.join();
			//m_threadHandle = NULL;

			m_root = path;
			m_callback = callback;
			m_flags = flags;

			int attributes = GetFileAttributesA(m_root.c_str());
			if (attributes == INVALID_FILE_ATTRIBUTES) {

			} else if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
				m_dirPath = m_root;
			} else {
				int index = m_root.find_last_of('\\');
				if (index == -1) {
					m_dirPath = ".\\";
				} else {
					m_dirPath = m_root.substr(0, index);
				}
			}
			m_changeHandle = FindFirstChangeNotificationA(m_dirPath.c_str(), flags & WATCH_SUBTREE,
				FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);
			if (m_changeHandle == INVALID_HANDLE_VALUE || m_changeHandle == NULL) {
				m_changeHandle = NULL;
				DWORD err = GetLastError();
				log::out << log::RED << "FileMonitor::check - invalid handle (err: " << (int)err << "): " << m_dirPath << "\n";
				m_mutex.unlock();
				return false;
			}

			//FILE_FLAG_OVERLAPPED
			m_dirHandle = CreateFileA(m_dirPath.c_str(), FILE_LIST_DIRECTORY,
				FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
			if (m_dirHandle == NULL || m_dirHandle == INVALID_HANDLE_VALUE) {
				m_dirHandle = NULL;
				DWORD err = GetLastError();
				log::out << log::RED << "FileMonitor::check - dirHandle failed(" << (int)err << "): " << m_dirPath << "\n";
				if (m_changeHandle != NULL) {
					FindCloseChangeNotification(m_changeHandle);
					m_changeHandle = NULL;
				}
				m_mutex.unlock();
				return false;
			}
			m_running = true;

			//log::out << "FileMonitor : Checking " << m_root << "\n";
			m_thread.init(RunMonitor,this);
		}
		m_mutex.unlock();
		return true;
	}
#ifdef VISUAL_STUDIO
	// Code below comes from https://learn.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code?view=vs-2022
	const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)
	void SetThreadName(DWORD dwThreadID, const char* threadName) {
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
		__try {
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		} __except (EXCEPTION_EXECUTE_HANDLER) {
		}
#pragma warning(pop)
	}
#else
	void SetThreadName(DWORD dwThreadID, const char* threadName) {
		// Empty
	}
#endif
}