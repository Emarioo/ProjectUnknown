#include "Engone/FileModule/FileWriter.h"

#include <vector>
#include <string>

#include <string.h>

namespace engone {
	FileWriter::FileWriter(const std::string& path, bool binaryForm) : binaryForm(binaryForm) {
		m_file = engone::FileOpen(path.c_str(),nullptr,false);
		if (!m_file) {
			m_error = FileNotFound;
			return;
		}
		m_path = path;
	}
	FileWriter::~FileWriter() {
		cleanup();
	}
	void FileWriter::close() {
		engone::FileClose(m_file);
		m_file = 0;
	}
	void FileWriter::cleanup() {
		m_path.clear();
		close();
		m_fileHead = 0;	
		m_error = NoError;
	}
	bool FileWriter::write(const void* ptr, uint64 bytes){
		Assert(ptr)

        uint64_t writtenBytes = FileWrite(m_file,(char*)ptr,bytes);
        if(writtenBytes!=bytes){
            m_error = OtherError;
            printf("FileWriter::write : bad error\n");
            return false;
        }
        return true;
    }
	uint64 FileWriter::writeNumbers(const char* ptr, uint64 count, uint typeSize, bool isFloat) {
		Assert(ptr)
		if (binaryForm) {
			return write(ptr, typeSize * count);
		}

        // Todo: improve the code
        char buffer[256];
        for(int i=0;i<count;i++){
            if(isFloat){
                double num = 0;
                if(typeSize==sizeof(float)){num=*((float*)ptr); ptr += typeSize;}
                else if(typeSize==sizeof(double)){num=*((double*)ptr); ptr += typeSize;}
                sprintf(buffer,"%lf ",num);
            }else{
                int64 num = 0;
                if(typeSize==sizeof(int8)){num = *((int8*)ptr); ptr += typeSize;}
                else if(typeSize==sizeof(int16)){num = *((int16*)ptr); ptr += typeSize;}
                else if(typeSize==sizeof(int32)){num = *((int32*)ptr); ptr += typeSize;}
                else if(typeSize==sizeof(int64)){num = *((int64*)ptr); ptr += typeSize;}
                sprintf(buffer,"%lld ",num);
            }
            uint64 bytesWritten = FileWrite(m_file, buffer, strlen(buffer));
            if(bytesWritten!=strlen(buffer)){
                m_error = OtherError;
                printf("Some error\n");
                return i;
            }
        }
        char buf='\n';
        // Todo: deal with error?
        FileWrite(m_file, &buf, 1);
        return count;
	}
	bool FileWriter::write(const std::string* ptr, uint64 count){
		if(binaryForm){
			uint16 length = ptr->size();
			bool yes = write(&length);
			if(!yes){
				return false;
			}
			yes = write(ptr->data(),length);
			if(!yes){
				return false;
			}
		}else{
			bool yes = write(ptr->data(),ptr->size());
			const char temp='\n';
			yes &= write(&temp,1);

			return yes;
		}
		return true;
	}
	#define GEN_WRITE_TYPE(TYPE,FLOATY)\
	bool FileWriter::write(const TYPE* ptr, uint64 count){\
		return count==writeNumbers((const char*)ptr,count,sizeof(TYPE),FLOATY);}\
		
	GEN_WRITE_TYPE(uint8,false)
	GEN_WRITE_TYPE(int16,false)
	GEN_WRITE_TYPE(uint16,false)
	GEN_WRITE_TYPE(int32,false)
	GEN_WRITE_TYPE(uint32,false)
	GEN_WRITE_TYPE(int64,false)
	GEN_WRITE_TYPE(uint64,false)
	GEN_WRITE_TYPE(float,true)
	GEN_WRITE_TYPE(double,true)
}