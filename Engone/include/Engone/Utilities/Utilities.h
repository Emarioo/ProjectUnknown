#pragma once

#include "Engone/Logger.h"
#include "Engone/LoopInfo.h"
#include "Engone/Utilities/Tracker.h"

namespace engone {

	// A context where data is stored
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
	//The time since epoch in seconds
	double GetSystemTime();

	namespace alloc {
		void* _malloc(uint64_t size);
		void* _realloc(void* ptr, uint64_t oldSize, uint64_t newSize);
		void _free(void* ptr, uint64_t size);
	}

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

	//class Timer {
	//public:
	//	Timer();
	//	Timer(const std::string& str);
	//	Timer(const std::string& str, int id);
	//	~Timer();
	//	void end();

	//private:
	//	double time;
	//	std::string name;
	//	int id = 0;
	//};

	// start will activate the class, run should be inside update loop which will update
	// the class's inner timer. Once reached, run will return true once otherwise false.
	// use start again to restart.
	class DelayCode {
	public:
		DelayCode() = default;

		void start(double waitInSeconds) { waitTime = waitInSeconds; running = true; }
		void stop() { running = false; }
		// info is here incase you want to slow down the entire app.
		// Without info, this class would not be affected.
		bool run(UpdateInfo& info) {
			if (running) { 
				time += info.timeStep;
				if (time >= waitTime) {
					running = false;
					return true;
				} 
			}
			return false; 
		}

	private:
		bool running = false;
		double time = 0;
		double waitTime = 0;
	};

	//#define TIMER(str) Timer str = Timer(#str,__LINE__*strlen(__FILE__))
	
	enum FormatTimePeriod : uint8_t {
		FormatTimeNS = 1, // nanoseconds
		FormatTimeUS = 2, // microseconds
		FormatTimeMS = 4, // milliseconds 
		FormatTimeS = 8, // seconds
		FormatTimeM = 16, // minutes
		FormatTimeH = 32, // hours
		FormatTimeD = 64, // days
		FormatTimeW = 128, // weeks
	};
	typedef int FormatTimePeriods;
	// if compact is true, result will be 1d 5h 1m 13s, false is 1 day 5 hours 1 minutes 13 seconds.
	// flags are used to determine what literals you want. day, hour, minute, second is default.
	// the smallest literal will be what time should be in. with default. time is in seconds
	// If you want precision, this function is best.
	// If you only use the day flag when the time is smaller than a day, an empty string will be returned.
	std::string FormatTime(uint64_t time, bool compact=false, FormatTimePeriods flags = FormatTimeD|FormatTimeH|FormatTimeM|FormatTimeS);
	// if compact is true, result will be 1d 5h 1m 13s, false is 1 day 5 hours 1 minutes 13 seconds.
	// flags are used to determine what literals you want. day, hour, minute, second is default.
	// If you want precision, use this function but with uin64_t in nanoseconds instead. Doubles are a bit special.
	// If you only use the day flag when the time is smaller than a day, an empty string will be returned.
	std::string FormatTime(double seconds, bool compact=false, FormatTimePeriods flags = FormatTimeD|FormatTimeH|FormatTimeM|FormatTimeS);
	
	// the time of your computer clock, i think 
	std::string GetClock();

	// instead std::this_thread::sleep_for, microsecond precision
	void Sleep(double seconds);

	// A random seet is set by default.
	void SetRandomSeed(uint32_t seed);
	uint32_t GetRandomSeed();
	// between 0 and 1
	double GetRandom();
	uint32_t Random32();
	uint64_t Random64();

	// random uuid of 16 bytes in total, use UUID::New to create one.
	class UUID {
	public:
		// memory is not initialized.
		UUID()=default;
		static UUID New();
		bool operator==(const UUID& uuid) const;
		bool operator!=(const UUID& uuid) const;

		// only intended for 0.
		UUID(const int num);

		// fullVersion as true will print the whole 16 bytes
		// otherwise the first 8 will be printed
		std::string toString(bool fullVersion=false) const;

	private:
		uint64_t data[2];

		friend struct std::hash<engone::UUID>;
	};
	log::logger operator<<(log::logger log,UUID uuid);

	//void CountingTest(int times, int numElements, std::function<int()> func);

	bool FindFile(const std::string& path);
	std::vector<std::string> GetFiles(const std::string& path);
	// In seconds
	uint64_t GetFileLastModified(const std::string& path);

	struct PNG {
		void* data=nullptr;
		uint32_t size=0;
	};
	// load png resource.
	PNG LoadPNG(int id);

	// Starts an exe at path. Uses CreateProcess from windows.h
	bool StartProgram(const std::string& path);

	// FILE
	enum FileError : char {
		FileErrorNone = 0,
		FileErrorMissing,
		FileErrorCorrupted,
	};
	const char* toString(FileError error);
	namespace log {
		logger operator<<(logger log, FileError err);
	}
	class FileWriter {
	public:
		FileWriter(const std::string& path, bool binaryForm = false) : binaryForm(binaryForm), path(path) {
			file.open(path, std::ios::binary);
			if (file) {

			} else {
				error = FileErrorMissing;
				close();
			}
		}
		~FileWriter() {
			close();
		}

		bool isOpen() const {
			return error == FileErrorNone;
		}
		bool operator!() const {
			return !isOpen();
		}
		operator bool() const {
			return isOpen();
		}
		void close() {
			if (file.is_open())
				file.close();
		}
		const std::string& getPath() const { return path; }
		FileError getError() const { return error; }
		/*
		T has to be convertable to a string std::to_string(T), otherwise cast T* to char* and multiply size by sizeof(T).
		*/
		template <typename T>
		void write(const T* var, uint32_t size = 1) {
			if (error != FileErrorNone || var == nullptr || size == 0)
				throw error;
			if (binaryForm) {
				file.write(reinterpret_cast<const char*>(var), size * (uint32_t)sizeof(T));
				writeHead += size * sizeof(T);
			} else {
				std::string out;
				for (uint32_t i = 0; i < size; i++) {
					if (i == 0)
						out += std::to_string(var[i]);
					else
						out += " " + std::to_string(var[i]);
				}
				file.write(out.c_str(), out.length());
				file.write("\n", 1);
			}
		}
		void write(const glm::vec3* var, uint32_t size = 1) {
			write((const float*)var, size * 3);
		}
		void write(const glm::mat4* var, uint32_t size = 1) {
			write((const float*)var, size * 16);
		}
		/*
		256 characters is the current max size.
		*/
		void write(const std::string* var) {
			if (error == FileErrorMissing)
				throw error;
			if (binaryForm) {
				file.write(var->c_str(), var->length());
				writeHead += var->length();
			} else {
				file.write(var->c_str(), var->length());
				file.write("\n", 1);
			}
		}
		void writeComment(const std::string& str) {
			if (error == FileErrorMissing)
				throw error;
			if (!binaryForm) {
				file.write("# ", 2);
				file.write(str.c_str(), str.length());
				file.write("\n", 1);
			}
		}
		static TrackerId trackerId;

	private:
		bool binaryForm = false;
		int writeHead = 0;
		std::ofstream file;
		std::string path;
		FileError error = FileErrorNone;

	};
	class FileReader {
	public:
		FileReader(const std::string& path, bool binaryForm = false) : binaryForm(binaryForm), path(path) {
			file.open(path, std::ios::binary);
			if (file) {
				file.seekg(0, file.end);
				fileSize = (std::uint32_t)file.tellg();
				file.seekg(file.beg);
			} else {
				error = FileErrorMissing;
			}
		}
		~FileReader() {
			close();
		}

		bool isOpen() const {
			return error == FileErrorNone;
		}
		bool operator!() const {
			return !isOpen();
		}
		operator bool() const {
			return isOpen();
		}
		uint32_t size() const {
			return fileSize;
		}
		void close() {
			if (file.is_open())
				file.close();
		}
		
		const std::string& getPath() const { return path; }
		FileError getError() const { return error; }
		/*
		T has to be convertable to int or float. Can also be char short and most of them. Maybe not long or double
		*/
		template <typename T>
		void read(T* var, uint32_t size = 1) {
			if (error != FileErrorNone || var == nullptr || size == 0)
				throw error;
			if (binaryForm) {
				if (readHead - 1 + size * sizeof(T) > fileSize) {
					error = FileErrorCorrupted;
					throw FileErrorCorrupted;
				}

				file.read(reinterpret_cast<char*>(var), size * (uint32_t)sizeof(T));
				readHead += size * sizeof(T);
			} else {
				try {
					std::string line;
					uint32_t index = 0;

					while (index < size) {
						if (readNumbers.size() == 0) {

							std::getline(file, line);

							if (file.eof())
								throw FileErrorCorrupted;
							if (line.empty())
								continue;
							if (line[0] == '#')
								continue;
							if (line.back() == '\r')
								line.erase(line.end() - 1);

							readNumbers = SplitString(line, " ");
						}
						uint32_t toRead = readNumbers.size();
						if (size < readNumbers.size())
							toRead = size;

						for (uint32_t i = 0; i < toRead; i++) {
							std::string& str = readNumbers[0];
							if (str.length() == 0) {
								readNumbers.erase(readNumbers.begin());
								continue;
							}
							if (str.find('.') == -1) {
								try {
									var[index] = static_cast<T>(std::stoi(str));
								} catch (std::out_of_range e) {
									var[index] = static_cast<T>(std::stoull(str));
								}
								index++;
							} else {
								var[index++] = static_cast<T>(std::stof(str));
							}
							readNumbers.erase(readNumbers.begin());
						}
					}
				} catch (std::invalid_argument e) {
					throw FileErrorCorrupted;
				}
			}
		}
		void read(glm::vec3* var, uint32_t size = 1) {
			read((float*)var, size * 3);
		}
		void read(glm::mat4* var, uint32_t size = 1) {
			read((float*)var, size * 16);
		}
		/*
		256 characters is the current max size.
		*/
		void read(std::string* var) {
			if (error != FileErrorNone || var == nullptr)
				throw error;
			if (binaryForm) {
				if (readHead - 1u + 1u > fileSize) {
					error = FileErrorCorrupted;
					throw error;
				}
				uint8_t length;
				file.read(reinterpret_cast<char*>(&length), 1);
				readHead++;

				if (length == 0u)
					return;

				if (readHead - 1u + length > fileSize) {
					error = FileErrorCorrupted;
					throw error;
				}

				*var = std::string(length, ' ');

				file.read(reinterpret_cast<char*>(var->data()), length);
				readHead += length;
			} else {
				std::string line;
				uint32_t index = 0;

				while (index < 1u) {
					if (file.eof()) {
						error = FileErrorCorrupted;
						throw error;
					}
					std::getline(file, line);

					//std::cout << line[0] << "\n";
					if (line[0] == '#') {
						continue;
					}
					if (!line.empty()) {
						if (line.back() == '\r')
							line.erase(line.end() - 1);
					}

					*var = line;
					index++;
				}
			}
		}
		void readAll(std::string* var) {
			if (error != FileErrorNone || var == nullptr)
				throw error;
			if (binaryForm) {
				log::out << log::RED << "readAll with binary is not implemented" << "\n";
				error = FileErrorCorrupted;
				throw error;
			} else {
				std::string line;
				while (true) {
					if (file.eof()) {
						break;
					}
					std::getline(file, line);

					if (line[0] == '#') {
						continue;
					}
					if (!line.empty()) {
						if (line.back() == '\r')
							line.erase(line.end() - 1);
					}
					*var += line + "\n";
				}
			}
		}
		// Does not include \n
		std::vector<std::string> readLines(bool includeNewLine=false) {
			if (error != FileErrorNone)
				return std::vector<std::string>();
			if (binaryForm) {
				log::out << log::RED << "readLines with binary is not implemented" << "\n";
				error = FileErrorCorrupted;
				throw error;
			} else {
				std::vector<std::string> list;
				std::string line;
				while (true) {
					if (file.eof()) {
						break;
					}
					std::getline(file, line);

					if (line[0] == '#') {
						continue;
					}
					if (!line.empty()) {
						if (line.back() == '\r')
							line.erase(line.end() - 1);
					}
					if(includeNewLine)
						list.push_back(line+"\n");
					else
						list.push_back(line);
				}
				return list;
			}
		}
		static TrackerId trackerId;
	private:
		bool binaryForm = false;
		uint32_t readHead = 0;
		uint32_t fileSize = 0;
		std::ifstream file;
		std::string path;
		FileError error = FileErrorNone;

		std::vector<std::string> readNumbers;
	};
}

template<>
struct std::hash<engone::UUID> {
	std::size_t operator()(const engone::UUID& u) const {
		return std::hash<uint64_t>{}(u.data[0]) ^ (std::hash<uint64_t>{}(u.data[1]) << 1);
	}
};