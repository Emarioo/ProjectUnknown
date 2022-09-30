
#ifndef ENGONE_FILEUTIL_GUARD
#define ENGONE_FILEUTIL_GUARD

#ifdef ENGONE_LOGGER
#include "Engone/Utilities/LoggingModule.h"
#endif
#include "Engone/Error.h"
#ifdef ENGONE_TRACKER
#include "Engone/Utilities/Tracker.h"
#endif
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
		inline void write(const glm::vec3* var, uint32_t count = 1) {
			write((const float*)var, count * 3);
		}
		inline void write(const glm::mat4* var, uint32_t count = 1) {
			write((const float*)var, count * 16);
		}
		/*
		256 characters is the current max size.
		*/
		void write(const std::string* var);
		void writeComment(const std::string& str);
		static TrackerId trackerId;

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

		inline bool isOpen() const {
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
		void read(glm::vec3* var, uint32_t count = 1) {
			read((float*)var, count * 3);
		}
		void read(glm::mat4* var, uint32_t count = 1) {
			read((float*)var, count * 16);
		}
		/*
		256 characters is the current max size.
		*/
		void read(std::string* var);
		void readAll(std::string* var);
		// Does not include \n
		std::vector<std::string> readLines(bool includeNewLine = false);
		static TrackerId trackerId;
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

#ifdef ENGONE_FILEUTIL_IMPL
#undef ENGONE_FILEUTIL_IMPL
	#ifdef ENGONE_LOGGER
	#include "Engone/Utilities/LoggingModule.h"
	#endif
namespace engone {
	TrackerId FileWriter::trackerId = "FileWriter";
	TrackerId FileReader::trackerId = "FileReader";
//	const char* toString(FileError e) {
//		switch (e) {
//		case FileErrorMissing: return "Missing File";
//		case FileErrorCorrupted: return "Corrupted Data";
//		}
//		return "";
//	}
//#ifdef ENGONE_LOGGER
//	Logger& operator<<(Logger& log, FileError value) {
//		return log << toString(value);
//	}
//#endif // ENGONE_LOGGER
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
	FileWriter::FileWriter(const std::string& path, bool binaryForm) : binaryForm(binaryForm), path(path) {
		file.open(path, std::ios::binary);
		if (!file.is_open()) {
			error = ErrorMissingFile;
		}
	}
	FileWriter::~FileWriter() {
		close();
	}
	void FileWriter::close() {
		if (file.is_open())
			file.close();
	}
	void FileWriter::write(const std::string* var) {
		if (error == ErrorMissingFile)
			throw error;
		if (binaryForm) {

			uint8_t length=var->size();
			file.write(reinterpret_cast<char*>(&length), 1);
			writeHead++;

			if (length == 0u)
				return;

			file.write(var->c_str(), var->length());
			writeHead += var->length();
		}
		else {
			file.write(var->c_str(), var->length());
			file.write("\n", 1);
		}
	}
	void FileWriter::writeComment(const std::string& str) {
		if (error == ErrorMissingFile)
			throw error;
		if (!binaryForm) {
			file.write("# ", 2);
			file.write(str.c_str(), str.length());
			file.write("\n", 1);
		}
	}
	FileReader::FileReader(const std::string& path, bool binaryForm) : binaryForm(binaryForm), path(path) {
		file.open(path, std::ios::binary);
		if (file.is_open()) {
			file.seekg(0, file.end);
			fileSize = (std::uint32_t)file.tellg();
			file.seekg(file.beg);
		}
		else {
			error = ErrorMissingFile;
		}
	}
	FileReader::~FileReader() {
		close();
	}
	void FileReader::close() {
		if (file.is_open())
			file.close();
	}
	void FileReader::read(std::string* var) {
		if (error != ErrorNone || var == nullptr)
			throw error;
		if (binaryForm) {
			if (readHead - 1u + 1u > fileSize) {
				error = ErrorCorruptedFile;
				throw error;
			}
			uint8_t length;
			file.read(reinterpret_cast<char*>(&length), 1);
			readHead++;

			if (length == 0u)
				return;

			if (readHead - 1u + length > fileSize) {
				error = ErrorCorruptedFile;
				throw error;
			}

			*var = std::string(length, ' ');

			file.read(reinterpret_cast<char*>(var->data()), length);
			readHead += length;
		}
		else {
			std::string line;
			uint32_t index = 0;

			while (index < 1u) {
				if (file.eof()) {
					error = ErrorCorruptedFile;
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
	void FileReader::readAll(std::string* var) {
		if (error != ErrorNone || var == nullptr) {
			log::out << "FileReader::readAll - " << error << "\n";
			throw error;
		}
		if (binaryForm) {
#ifdef ENGONE_LOGGER
			log::out << log::RED << "readAll with binary is not implemented" << "\n";
#endif
			error = ErrorCorruptedFile;
			throw error;
		}
		else {
			var->clear();
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
	std::vector<std::string> FileReader::readLines(bool includeNewLine) {
		if (error != ErrorNone)
			return std::vector<std::string>();
		if (binaryForm) {
#ifdef ENGONE_LOGGER
			log::out << log::RED << "readLines with binary is not implemented" << "\n";
#endif
			error = ErrorCorruptedFile;
			throw error;
		}
		else {
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
				if (includeNewLine)
					list.push_back(line + "\n");
				else
					list.push_back(line);
				}
			return list;
			}
		}
}
#endif // ENGONE_FILEUTIL_IMPL