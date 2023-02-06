#pragma once

#include "Engone/PlatformModule/PlatformModule.h"
#include "Engone/Utilities/Alloc.h"
#include "Engone/Error.h"

namespace engone {

	// How to deal with buffers? I believe Windows does some buffering.
	// The reader is currently allocating a buffer for the whole file
	// and reading everything at once. You may want to do something different for larger files.
	class FileReader {
	public:
		FileReader(const std::string& path, bool binaryForm = false);
		~FileReader();
		void cleanup();

		FileReader(const FileReader&) = delete;
		FileReader& operator=(const FileReader&) = delete;

		inline bool isOpen() const {return m_error == ErrorNone;}
		inline bool operator!() const {return !isOpen();}
		inline operator bool() const {return isOpen();}
		inline uint32_t size() const {return m_fileSize;}

		// Data in buffer can still be read if file is closed.
		void close();

		inline const std::string& getPath() const { return m_path; }
		inline Error getError() const { return m_error; }

		bool read(char* var, uint32 bytes);
		bool readLine(std::string& line);
		bool readNumbers(char* var, uint32 intSize, uint32 count);
		/*
		T has to be convertable to int or float. Can also be char short and most of them. Maybe not long or double
		*/
		template <typename T>
		bool read(T* var, uint32 count = 1) {
			return readNumbers((char*)var,sizeof(T),count);
		}
		template <typename T>
		bool read(T& var) {
			return read(&var, 1);
		}
		bool read(glm::vec3* var, uint32 count = 1) {
			return read((float*)var, count * 3);
		}
		bool read(glm::mat4* var, uint32 count = 1) {
			return read((float*)var, count * 16);
		}
		bool read(std::string& var);
		void readAll(std::string& var);
		// Does not include \n
		std::vector<std::string> readLines(bool includeNewLine = false);
	private:
		bool binaryForm = false;

		std::string m_path;
		APIFile m_file;
		uint64 m_readHead=0;
		uint64 m_bufferReadHead=0;
		uint64 m_fileSize=0;
		Error m_error = ErrorNone;
		
		Memory<char> buffer{};
	};
}