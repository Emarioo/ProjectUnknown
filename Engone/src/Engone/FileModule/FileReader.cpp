#include "Engone/FileModule/FileReader.h"

#include <vector>
#include <string>

// #include "Engone/Logger.h"
namespace engone {
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
		m_file = engone::FileOpen(path.c_str(),&m_fileSize,true);
		if (!m_file) {
			m_error = FileNotFound;
			return;
		}
		bool yes = m_buffer.resize(BYTES_PER_READ);
		if (!yes) {
			m_error = AllocFailure;
			return;
		}
		m_path = path;
		uint64 bytes = engone::FileRead(m_file, m_buffer.data, BYTES_PER_READ);
		m_buffer.used=bytes;
		
		if(bytes==m_fileSize){
			close();
		}
	}
	FileReader::~FileReader() {
		cleanup();
	}
	void FileReader::close() {
		engone::FileClose(m_file);
		m_file = 0;
	}
	void FileReader::cleanup() {
		m_path.clear();
		close();
		m_fileSize = 0;
		m_fileHead = 0;	
		m_error = NoError;
		m_bufferHead = 0;
		m_buffer.resize(0);
	}
	bool FileReader::read(void* ptr, uint64 bytes){
		Assert(ptr)
		uint64 bytesInBuffer = m_buffer.used - m_bufferHead;
		if(bytesInBuffer >= bytes){
			// Bytes exist in buffer, read them
			memcpy(ptr,m_buffer.data,bytes);
			m_bufferHead += bytes;
			return true;
		}
		
		uint64 bytesInFile = m_fileSize - m_fileHead;
		if(bytesInBuffer+bytesInFile < bytes || !m_file){
			m_error = EndOfFile;
			return false; // not enough bytes in buffer and file
		}
		
		memcpy(ptr,m_buffer.data,bytesInBuffer);
		char* outPtr = (char*)ptr+bytesInBuffer;
		m_bufferHead = 0;
		
		uint64 bytesLeft = bytes-bytesInBuffer;
		while(true){
			uint64 readBytes = FileRead(m_file,m_buffer.data,BYTES_PER_READ);
			if(readBytes==-1){
				// there should be bytes left, we checked m_fileSize, something else is going on
				m_error = DataWasLost;
				Assert(("FileReader : DataWasLost",false));
				return false;
			}
			m_fileHead += readBytes;
			m_buffer.used = readBytes;
			
			if(readBytes>=bytesLeft){
				// we done
				memcpy(outPtr,m_buffer.data,bytesLeft);
				m_bufferHead += bytesLeft;
				return true;
			} else if(readBytes==BYTES_PER_READ){
				// cool but we need more	
				memcpy(outPtr,m_buffer.data,BYTES_PER_READ);
				outPtr+=BYTES_PER_READ;
			} else {
				// no more ):
				m_error = DataWasLost;
				return false;
			}
		}
		return false;
	}
		
	uint64 FileReader::readLine(std::string& line) {
		line.clear();
		while(true) {
			int bytesLeft = m_buffer.used-m_bufferHead;
			Assert(bytesLeft>=0)
			
			if(bytesLeft==0){
				uint64 readBytes = 0;
				if(m_file)
					readBytes = FileRead(m_file,m_buffer.data,BYTES_PER_READ);
					
				if(readBytes==-1){
					// Todo: Use a different error when m_file == 0?
					m_error = EndOfFile;
					break;
				}	
				m_fileHead += readBytes;
				m_buffer.used = readBytes;
				m_bufferHead = 0;
				if(readBytes==0){
					m_error = EndOfFile;
					break; // no more bytes to read, end of line
				}
			}
			
			if(m_buffer.data[m_bufferHead]=='\n'){
				m_bufferHead++;
				break;
			}
			if(m_buffer.data[m_bufferHead]=='\r'){
				m_bufferHead++;
				continue;
			}
			
			line += m_buffer.data[m_bufferHead++];
		}
		return line.length();
	}
	uint64 FileReader::readNumbers(char* ptr, uint64 count, uint typeSize, bool isFloat) {
		Assert(ptr)
		if (binaryForm) {
			return read(ptr, typeSize * count);
		}
		
		int numberCount=0;
		std::string number;
		while(numberCount!=count){
			int bytesLeft = m_buffer.used-m_bufferHead;
			Assert(bytesLeft>=0)
			
			bool lastNumber=false;
			bool endNumber=false;
			if(bytesLeft==0){
				uint64 readBytes = 0;
				if(m_file)
					readBytes = FileRead(m_file,m_buffer.data,BYTES_PER_READ);
					
				if(readBytes==-1){
					// Todo: Use a different error when m_file == 0?
					m_error = EndOfFile;
					return false;
				}	
				m_buffer.used = readBytes;
				m_bufferHead=0;
				m_fileHead += readBytes;
				if(readBytes==0){
					lastNumber=true;
					endNumber=true;
					m_error = EndOfFile;
				}
			}
			if(!lastNumber){
				char chr = m_buffer.data[m_bufferHead];
				m_bufferHead++;
				if(chr=='\r'){ // just skip
					continue;
				}
				if(chr=='\n' || chr==' '){
					endNumber=true;
				}else{
					number+=chr;
					continue;
				}
			}
			
			if(endNumber&&!number.empty()){
				numberCount++;
				double num = atof(number.c_str());
			
				if(isFloat){
					if(typeSize==sizeof(float)){*((float*)ptr) = num;ptr += typeSize;}
					else if(typeSize==sizeof(double)){*((double*)ptr) = num;ptr += typeSize;}
				}else{
					if(typeSize==sizeof(int8)){*((int8*)ptr) = num;ptr += typeSize;}
					else if(typeSize==sizeof(int16)){*((int16*)ptr) = num;ptr += typeSize;}
					else if(typeSize==sizeof(int32)){*((int32*)ptr) = num;ptr += typeSize;}
					else if(typeSize==sizeof(int64)){*((int64*)ptr) = num;ptr += typeSize;}
				}
				
				number.clear();
			}
			if(lastNumber)
				break;
		}
		return numberCount;
	}
	#define GEN_READ_TYPE(TYPE,FLOATY)\
	bool FileReader::read_##TYPE(TYPE* ptr, uint64 count){\
		return count==readNumbers((char*)ptr,count,sizeof(TYPE),FLOATY);}\
		
	GEN_READ_TYPE(int16,false)
	GEN_READ_TYPE(int,false)
	GEN_READ_TYPE(int64,false)
	GEN_READ_TYPE(float,true)
	GEN_READ_TYPE(double,true)
}