
#include "Engone/Error.h"
#include "Engone/PlatformModule/PlatformLayer.h"

namespace engone{
	class FileWriter {
	public:
		FileWriter(const std::string& path, bool binaryForm = false);
		~FileWriter();
		void cleanup();
		void close();
		
		FileWriter(const FileWriter&) = delete;
		FileWriter& operator=(const FileWriter&) = delete;

		inline bool isOpen() const {return m_error == NoError;}
		inline bool operator!() const {return !isOpen();}
		inline operator bool() const {return isOpen();}
		
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
		APIFile* m_file = 0;
		uint64 m_fileHead = 0;
		std::string path;
		Error m_error = NoError;

	};   
}