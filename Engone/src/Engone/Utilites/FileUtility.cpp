#include "Engone/Utilities/FileUtility.h"


#ifdef ENGONE_LOGGER
#include "Engone/Logger.h"
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

			uint8_t length = var->length();
			file.write(reinterpret_cast<char*>(&length), 1);
			writeHead++;

			if (length == 0u)
				return;

			file.write(var->c_str(), var->length());
			writeHead += var->length();
		} else {
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
		} else {
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
		if (error != ErrorNone)
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

			var->resize(length);

			file.read(reinterpret_cast<char*>(var->data()), length);
			readHead += length;
		} else {
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
		// works the same for both forms
		var->clear();
		var->resize(fileSize);
		file.read(var->data(), fileSize);
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
				if (includeNewLine)
					list.push_back(line + "\n");
				else
					list.push_back(line);
			}
			return list;
		}
	}
}