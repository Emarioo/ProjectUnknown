
#include "Engone/Utilities/Utilities.h"

#include "Engone/vendor/stb_image/stb_image.h"

namespace engone {
	TrackerId ItemVector::trackerId = "ItemVector";

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
		if (lastAt != text.size()||lastAt==0)
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
				GetTracker().subMemory<ItemVector>(m_maxSize);
				m_data = nullptr;
				m_maxSize = 0;
				m_writeIndex = 0;
				m_readIndex = 0;
			}
		}
		else {
			if (!m_data) {
				char* newData = (char*)alloc::malloc(size);
				if (!newData) {
					log::out << log::RED << "ItemVector failed allocation memory\n";
					return false;
				}
				GetTracker().addMemory<ItemVector>(size);
				m_data = newData;
				m_maxSize = size;
			}
			else {
				char* newData = (char*)alloc::realloc(m_data, m_maxSize, size);
				if (!newData) {
					log::out << log::RED << "ItemVector failed reallocating memory\n";
					return false;
				}
				GetTracker().subMemory<ItemVector>(m_maxSize);
				GetTracker().addMemory<ItemVector>(size);
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
	void DelayCode::Stop(int id, float waitSeconds){
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
	bool DelayCode::Run(int id, UpdateInfo& info) {
		return Run(id,info.timeStep);
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
	void ConvertWide(const std::string& in,std::wstring& out) {
		out.resize(in.length(), 0);
		for (int i = 0; i < in.length(); i++) {
			out.data()[i] = in[i];
		}
	}
	void ConvertWide(const std::wstring& in, std::string& out) {
		out.resize(in.length(),0);
		for (int i = 0; i < in.length(); i++) {
			out.data()[i] = in[i];
		}
	}
	bool StartProgram(const std::string& path) {
		if (!FindFile(path)) {
			return false;
		}

		// additional information
		STARTUPINFO si;
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
			NULL,        // Command line
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
		} else if (ptr > m_value) {
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
		} else if (ptr > m_value) {
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
}