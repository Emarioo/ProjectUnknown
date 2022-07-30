
#include "Engone/Utilities/Utilities.h"

namespace engone {
	TrackerId ItemVector::trackerId="ItemVector";
	TrackerId FileWriter::trackerId="FileWriter";
	TrackerId FileReader::trackerId="FileReader";


	std::vector<std::string> SplitString(std::string text, std::string delim) {
		std::vector<std::string> out;
		unsigned int at = 0;
		while ((at = text.find(delim)) != std::string::npos) {
			std::string push = text.substr(0, at);
			//if (push.size() > 0) {
				out.push_back(push);
			//}
			text.erase(0, at + delim.length());
		}
		if(text.size()!=0)
			out.push_back(text);
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
			d -= glm::pi<float>()*2;
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

	// double in seconds, microsecond precision
	double GetSystemTime() {
		return (double)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()/1000000.0;
		//std::cout << std::chrono::system_clock::now().time_since_epoch().count() <<" "<< (std::chrono::system_clock::now().time_since_epoch().count() / 10000000) << "\n";
		//return (double)(std::chrono::system_clock::now().time_since_epoch().count() / 10000000);
	}
	
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
				alloc::_free(m_data, m_maxSize);
				GetTracker().subMemory<ItemVector>(m_maxSize);
				m_data = nullptr;
				m_maxSize = 0;
				m_writeIndex = 0;
				m_readIndex = 0;
			}
		} else {
			if (!m_data) {
				char* newData = (char*)alloc::_malloc(size);
				if (!newData) {
					log::out << log::RED << "ItemVector failed allocation memory\n";
					return false;
				}
				GetTracker().addMemory<ItemVector>(size);
				m_data = newData;
				m_maxSize = size;
			} else {
				char* newData = (char*)alloc::_realloc(m_data, m_maxSize, size);
				if (!newData) {
					log::out << log::RED << "ItemVector failed reallocating memory\n";
					return false;
				}
				GetTracker().addMemory<ItemVector>(size - m_maxSize);
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
	void Sleep(double seconds) {
		std::this_thread::sleep_for((std::chrono::microseconds)((uint64_t)(seconds*1000000)));
	}
	//-- Random stuff
	static std::mt19937 randomGenerator;
	static uint32_t randomSeed=0;
	static bool seedonce = false;
	static void initGenerator() {
		if (!seedonce) {
			SetRandomSeed((uint32_t)GetSystemTime());
		}
	}
	static std::uniform_real_distribution<double> random_standard(0.0,1.0);
	static std::uniform_int_distribution<uint32_t> random_32(0, -1);
	void SetRandomSeed(uint32_t seed) {
		seedonce = true;
		randomSeed=seed;
		randomGenerator.seed(seed);
	}
	uint32_t GetRandomSeed() {
		return randomSeed;
	}
	double GetRandom() {
		initGenerator();
		return random_standard(randomGenerator);
	}
	uint32_t Random32() {
		initGenerator();
		return random_32(randomGenerator);
	}
	uint64_t Random64() {
		initGenerator();
		return ((uint64_t)random_32(randomGenerator)<<32)
			|(uint64_t)random_32(randomGenerator);
	}
	UUID UUID::New(){
		uint64_t out[2];
		for (int i = 0; i < 2; i++) {
			out[i] = Random64();
		}
		return *(UUID*)&out;
	}
	UUID::UUID(const int num) {
		// Uncomment debugbreak, run program, check stackframe.
		if (num != 0) DebugBreak();
		assert(("UUID was created from non 0 int which is not allowed. Put a breakpoint here and check stackframe.", num == 0));
		memset(this, 0, sizeof(UUID));
	}
	bool UUID::operator==(const UUID& uuid) const {
		for (int i = 0; i < 2; i++) {
			if (data[i] != uuid.data[i])
				return false;
		}
		return true;
	}
	bool UUID::operator!=(const UUID& uuid) const {
		return !(*this==uuid);
	}
	char toHex(uint8_t num) {
		if (num < 10) return '0' + num;
		else return 'A' + num - 10;
	}
	std::string UUID::toString(bool fullVersion) const {
		char out[2*sizeof(UUID)+4+1]; // maximum hash string, four dashes, 1 null
		
		uint8_t* bytes = (uint8_t*)this;

		int write = 0;
		for (int i = 0; i < sizeof(UUID);i++) {
			if (i == sizeof(UUID) / 2&& !fullVersion)
				break;
			if (i == 4||i==6||i==8||i==10)
				out[write++] = '-';
			out[write++] = toHex(bytes[i]&15);
			out[write++] = toHex(bytes[i] >> 4);
		}
		out[write] = 0; // finish with null terminated char
		return out;
	}
	log::logger operator<<(log::logger log, UUID uuid) {
		log << uuid.toString();
		return log;
	}

	std::string GetClock() {
		time_t t;
		time(&t);
		tm tm;
		localtime_s(&tm, &t);
		std::string str = std::to_string(tm.tm_hour) + ":" + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec);
		return str;
	}
	std::string FormatTime(double seconds, bool compact, FormatTimePeriods flags) {
		bool small = false;
		for (int i = 0; i < 3;i++) {
			if (flags & (1 << i))
				small = true;
			if (small) seconds *= 1000;
		}
		return FormatTime((uint64_t)round(seconds),compact,flags);
	}
	std::string FormatTime(uint64_t time, bool compact, FormatTimePeriods flags) {
		// what is the max amount of characters?
		const int outSize = 130;
		char out[outSize]{};
		int write = 0;

		uint64_t num[8]{};
		uint64_t divs[8]{ 1,1,1,1,1,1,1,1 };
		uint64_t divLeaps[8]{ 1000,1000,1000,60,60,24,7,1 };
		const char* lit[8]{"nanosecond","microsecond","millisecond","second","minute","hour","day","week"};

		uint64_t rest = time;

		bool smallest = false;
		for (int i = 0; i < 7; i++) {
			if (flags & (1 << i))
				smallest = true;
			if (smallest)
				divs[i + 1] *= divs[i]*divLeaps[i];
		}

		for (int i = 7; i >= 0;i--) {
			if (0==(flags & (1 << i))) continue;
			num[i] = rest/divs[i];
			if (num[i] == 0) continue;
			rest -= num[i] * divs[i];

			if (write != 0) out[write++] = ' ';
			write += snprintf(out + write, outSize - write, "%u ", num[i]);
			if (compact) {
				out[write++] = lit[i][0];
				if (i < 3)
					out[write++] = 's';
			} else {
				memcpy(out + write, lit[i], strlen(lit[i]));
				write += strlen(lit[i]);
				if(num[i]!=1)
					out[write++] = 's';
			}
		}
		return out;
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

	//static std::unordered_map<int, int> timerCounting;
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
	bool FindFile(const std::string& path) {
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}
	std::vector<std::string> GetFiles(const std::string& path) {
		std::vector<std::string> list;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			list.push_back(entry.path().generic_string());
		}
		return list;
	}
	// In seconds
	uint64_t GetFileLastModified(const std::string& path) {
		if (std::filesystem::exists(path))
			return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(path).time_since_epoch()).count();
		//log::out << log::RED << "getTime - path not valid\n";
		return 0;
	}
	static wchar_t buffer[256]{};// Will this hardcoded size be an issue?
	std::wstring convert(const std::string& in) {
		ZeroMemory(buffer, 256);
		for (int i = 0; i < in.length(); i++) {
			buffer[i] = in[i];
		}
		return buffer;
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

#ifdef NDEBUG
		std::wstring exeFile = convert(path);
		std::wstring workDir = convert(workingDir);
#else
		const std::string& exeFile = path;
		std::string& workDir = workingDir;
#endif
		CreateProcess(exeFile.c_str(),   // the path
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

	PNG LoadPNG(int id) {
		HRSRC hs = FindResource(NULL, MAKEINTRESOURCE(id), "PNG");
		HGLOBAL hg = LoadResource(NULL, hs);
		void* ptr = LockResource(hg);
		DWORD size = SizeofResource(NULL, hs);
		return { ptr, size };
	}

	// Currently the same as normal malloc, realloc and free.
	namespace alloc {
		void* _malloc(uint64_t size) {
			if (size == 0) return nullptr;
			void* ptr = malloc(size);
			return ptr;
		}
		void* _realloc(void* ptr, uint64_t oldSize, uint64_t newSize) {
			if (newSize == 0) {
				_free(ptr,oldSize);
				return nullptr;
			}
			void* newPtr = realloc(ptr, newSize);
			if (newPtr) {
				return newPtr;
			} else {
				return ptr;
			}
		}
		void _free(void* ptr, uint64_t size) {
			if (!ptr) return;
			free(ptr);
		}
	}

	// FILE
	const char* toString(FileError e) {
		switch (e) {
		case FileErrorMissing: return "Missing File";
		case FileErrorCorrupted: return "Corrupted Data";
		}
		return "";
	}
	namespace log {
		logger operator<<(logger log, FileError err) {
			return log << toString(err);
		}
	}
}