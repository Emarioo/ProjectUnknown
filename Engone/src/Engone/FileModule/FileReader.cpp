#include "Engone/FileModule/FileReader.h"


#ifdef ENGONE_LOGGER
#include "Engone/Logger.h"
#endif
namespace engone {
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
	//bool FindFile(const std::string& path) {
	//	struct stat buffer;
	//	return (stat(path.c_str(), &buffer) == 0);
	//}
	//std::vector<std::string> GetFiles(const std::string& path) {
	//	std::vector<std::string> list;
	//	for (const auto& entry : std::filesystem::directory_iterator(path)) {
	//		list.push_back(entry.path().generic_string());
	//	}
	//	return list;
	//}
	//// In seconds
	//uint64_t GetFileLastModified(const std::string& path) {
	//	if (std::filesystem::exists(path))
	//		return std::chrono::duration_cast<std::chrono::seconds>(std::filesystem::last_write_time(path).time_since_epoch()).count();
	//	//log::out << log::RED << "getTime - path not valid\n";
	//	return 0;
	//}
	FileReader::FileReader(const std::string& path, bool binaryForm) : binaryForm(binaryForm) {
		m_file = engone::OpenFile(path.c_str(),&m_fileSize);
		if (!m_file) {
			m_error = FileNotFound;
			return;
		}
		bool yes = buffer.resize(m_fileSize);
		if (!yes) {
			m_error = AllocFailure;
			return;

		}
		m_path = path;
		uint64 bytes = engone::ReadFile(m_file, buffer.data, m_fileSize);
		Assert(bytes==m_fileSize);

		close();
	}
	FileReader::~FileReader() {
		close();
		cleanup();
	}
	void FileReader::close() {
		engone::CloseFile(m_file);
		m_file = 0;
	}
	void FileReader::cleanup() {
		buffer.resize(0);
	}
	bool FileReader::read(char* var, uint32 bytes) {
		if (var == nullptr)
			return false;
		if (bytes == 0)
			return true;

		if (m_fileSize < m_bufferReadHead + bytes) {
			m_error = EndOfFile;
			return false;
		}
		
		memcpy(var,buffer.data+m_bufferReadHead,bytes);
		m_readHead += bytes;
		m_bufferReadHead += bytes;
		return false;
	}
	bool FileReader::read(std::string& var) {
		if(!binaryForm)
			readLine(var);
		else {
			uint8 length;
			bool yes =read(&length);
			if (!yes)
				return false;

			if (length == 0u)
				return true;

			if (buffer.used < m_bufferReadHead + length) {
				m_error = EndOfFile;
				return false;
			}

			var.resize(length);

			memcpy(buffer.data+m_bufferReadHead,var.data(),length);
			m_bufferReadHead += length;
			m_readHead += length;
		}
	}
	bool FileReader::readLine(std::string& line) {
		line.clear();
		while(true) {
			if (buffer.used<=m_bufferReadHead) {
				return line.size()!=0;
			}
			if (buffer.data[m_bufferReadHead] == '\n') {
				return true;
			}
			if(buffer.data[m_bufferReadHead]!='\r')
				line += buffer.data[m_bufferReadHead];
			m_bufferReadHead++;
		}
	}
	bool FileReader::readNumbers(char* var, uint32 intSize, uint32 count) {
		if (var == nullptr)
			return false;
		if (count == 0 || intSize == 0) {
			return true;
		}
		if (binaryForm) {
			return read(var, intSize * count);
		} else {
			std::string line;
			std::vector<std::string> numbers;
			while (numbers.size() < count) {
				// requires buffer
				bool yes = readLine(line);
				
				if(!yes) {
					m_error = EndOfFile;
					return false;
				}
				if (line[0] == '#')
					continue;
				if (line.back() == '\r')
					line.erase(line.end() - 1);

				// one two three
				const std::string delim = " ";
				int lastAt = 0;
				while (true) {
					int at = line.find(delim, lastAt);
					if (at == -1) {
						break;
					}
					std::string push = line.substr(lastAt, at - lastAt);
					numbers.push_back(push);
					lastAt = at + 1;
				}
				if (lastAt != line.size() || lastAt == 0)
					numbers.push_back(line.substr(lastAt));
				//readNumbers = SplitString(line, " "); // could use this instead but that would mean this header requires Utilities.h.
			}

			uint32 index = 0;
			for (auto& num : numbers) {
				if (num.find('.') == -1) {
					// not decimal
					if (intSize == sizeof(int8)) {
						try {
							((int8*)var)[index] = std::stoi(num);
						} catch (std::out_of_range e) {
							((int8*)var)[index] = std::stoull(num);
						}
					}else if (intSize == sizeof(int16)) {
						try {
							((int16*)var)[index] = std::stoi(num);
						} catch (std::out_of_range e) {
							((int16*)var)[index] = std::stoull(num);
						}
					}else  if (intSize == sizeof(int32)) {
						try {
							((int32*)var)[index] = std::stoi(num);
						} catch (std::out_of_range e) {
							((int32*)var)[index] = std::stoull(num);
						}
					}else if (intSize == sizeof(int64)) {
						try {
							((int64*)var)[index] = std::stoi(num);
						} catch (std::out_of_range e) {
							((int64*)var)[index] = std::stoull(num);
						}
					}
				} else {
					if (intSize == sizeof(float)) {
						((float*)var)[index++] = std::stof(num);
					} else if (intSize == sizeof(double)) {
						((double*)var)[index++] = std::stof(num);
					}
				}
			}
		}
	}
	void FileReader::readAll(std::string& var) {
		// works the same for both forms
		var.clear();
		uint64 bytes = m_fileSize - m_readHead;
		var.resize(bytes);
		
		memcpy(var.data(),buffer.data,bytes);
	}
	std::vector<std::string> FileReader::readLines(bool includeNewLine) {
		std::vector<std::string> lines;
		std::string line;
		while (true) {
			bool yes = readLine(line);
			if (!yes) break;
			if(includeNewLine)
				line += "\n";
			lines.push_back(line);
		}
		return lines;
	}
}