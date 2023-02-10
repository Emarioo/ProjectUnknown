
#ifndef ENGONE_FILEUTIL_GUARD
#define ENGONE_FILEUTIL_GUARD

// #define ENGONE_TRACKER
#ifdef ENGONE_LOGGER
#include "Engone/Logger.h"
#endif
#include "Engone/Error.h"
// #ifdef ENGONE_TRACKER
// #include "Engone/Utilities/Tracker.h"
// #endif
namespace engone {
	// FILE
	//enum FileError : char {
	//	FileErrorNone = 0,
	//	FileErrorMissing,
	//	FileErrorCorrupted,
	//};
//	const char* toString(FileError error);
//#ifdef ENGONE_LOGGER
//	Logger& operator<<(Logger& log, FileError value);
//#endif

	bool FindFile(const std::string& path);
	std::vector<std::string> GetFiles(const std::string& path);
	// In seconds
	uint64_t GetFileLastModified(const std::string& path);

	class FileWriter {
	public:
		FileWriter(const std::string& path, bool binaryForm = false);
		~FileWriter();

		FileWriter(const FileWriter&) = delete;
		FileWriter& operator=(const FileWriter&) = delete;

		inline bool isOpen() const {
			return error == ErrorNone;
		}
		inline bool operator!() const {
			return !isOpen();
		}
		inline operator bool() const {
			return isOpen();
		}
		void close();
		inline const std::string& getPath() const { return path; }
		inline Error getError() const { return error; }
		/*
		T has to be convertable to a string std::to_string(T), otherwise cast T* to char* and multiply size by sizeof(T).
		*/
		template <typename T>
		void write(const T* var, uint32_t count = 1) {
			if (error != ErrorNone || var == nullptr)
				throw error;
			if (count == 0)
				return;
			if (binaryForm) {
				file.write(reinterpret_cast<const char*>(var), count * (uint32_t)sizeof(T));
				writeHead += count * sizeof(T);
			}
			else {
				std::string out;
				for (uint32_t i = 0; i < count; i++) {
					if (i == 0)
						out += std::to_string(var[i]);
					else
						out += " " + std::to_string(var[i]);
				}
				file.write(out.c_str(), out.length());
				file.write("\n", 1);
			}
		}
		//template <typename T>
		//void write(const T& var) {
		//	write(&var, 1);
		//}
		template <typename T>
		void writeOne(T var) {
			write(&var, 1);
		}
		inline void write(const glm::vec3* var, uint32_t count = 1) {
			write((const float*)var, count * 3);
		}
		inline void write(const glm::mat4* var, uint32_t count = 1) {
			write((const float*)var, count * 16);
		}
		/*
		256 characters is the current max size.
		*/
		void write(const std::string& var) {
			write(&var);
		}
		void write(const std::string* var);
		void writeComment(const std::string& str);
		// static TrackerId trackerId;

	private:
		bool binaryForm = false;
		int writeHead = 0;
		std::ofstream file;
		std::string path;
		Error error = ErrorNone;

	};
	class FileReader {
	public:
		FileReader(const std::string& path, bool binaryForm = false);
		~FileReader();

		FileReader(const FileReader&) = delete;
		FileReader& operator=(const FileReader&) = delete;

		inline bool isOpen() const { // bad name? file is still open if corruption happened.
			return error == ErrorNone;
		}
		inline bool operator!() const {
			return !isOpen();
		}
		inline operator bool() const {
			return isOpen();
		}
		inline uint32_t size() const {
			return fileSize;
		}
		void close();

		inline const std::string& getPath() const { return path; }
		inline Error getError() const { return error; }
		/*
		T has to be convertable to int or float. Can also be char short and most of them. Maybe not long or double
		*/
		template <typename T>
		void read(T* var, uint32_t count = 1) {
			if (error != ErrorNone || var == nullptr)
				throw error;
			if (count == 0) {
				return;
			}
			if (binaryForm) {
				if (readHead + count * sizeof(T) > fileSize) {
					error = ErrorCorruptedFile;
					throw error;
				}

				file.read(reinterpret_cast<char*>(var), count * (uint32_t)sizeof(T));
				readHead += count * sizeof(T);
			}
			else {
				try {
					std::string line;
					uint32_t index = 0;

					while (index < count) {
						if (readNumbers.size() == 0) {

							std::getline(file, line);

							if (file.eof()) {
								error = ErrorCorruptedFile;
								throw error;
							}
							if (line.empty())
								continue;
							if (line[0] == '#')
								continue;
							if (line.back() == '\r')
								line.erase(line.end() - 1);

							// one two three
							const std::string delim = " ";
							int lastAt = 0;
							while (true) {
								int at = line.find(delim,lastAt);
								if (at == -1) {
									break;
								}
								std::string push = line.substr(lastAt, at-lastAt);
								readNumbers.push_back(push);
								lastAt = at+1;
							}
							if(lastAt!=line.size() || lastAt == 0)
								readNumbers.push_back(line.substr(lastAt));
							//readNumbers = SplitString(line, " "); // could use this instead but that would mean this header requires Utilities.h.
						}
						uint32_t toRead = readNumbers.size();
						if (count < readNumbers.size())
							toRead = count;

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
							}
							else {
								var[index++] = static_cast<T>(std::stof(str));
							}
							readNumbers.erase(readNumbers.begin());
						}
					}
				}
				catch (std::invalid_argument e) {
					error = ErrorCorruptedFile;
					throw error;
				}
			}
		}
		template <typename T>
		void readOne(T& var) {
			read(&var, 1);
		}
		void read(glm::vec3* var, uint32_t count = 1) {
			read((float*)var, count * 3);
		}
		void read(glm::mat4* var, uint32_t count = 1) {
			read((float*)var, count * 16);
		}
		/*
		256 characters is the current max size.
		*/
		void read(std::string& var) {
			read(&var);
		}
		void read(std::string* var);
		void readAll(std::string* var);
		// Does not include \n
		std::vector<std::string> readLines(bool includeNewLine = false);
		// static TrackerId trackerId;
	private:
		bool binaryForm = false;
		uint32_t readHead = 0;
		uint32_t fileSize = 0;
		std::ifstream file;
		std::string path;
		Error error = ErrorNone;

		std::vector<std::string> readNumbers;
	};
}
#endif // ENGONE_FILEUTIL_GUARD