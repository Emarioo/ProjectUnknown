
#include "Engone/Sound/Libaudio.h"

#include "Engone/PlatformLayer.h"
#include "Engone/Logger.h"


i8* ReadWAVE(const std::string& filename, int& channels, int& sampleRate, int& bps, int& size) {
	char buffer[4];
	
	WAVEHeader header{};

	u64 fileSize = 0;
	auto file = engone::FileOpen(filename, engone::FILE_READ_ONLY, &fileSize);
	Assert(file);
	if(!file)
		return nullptr;
	Assert(fileSize >= WAVEHeader::SIZE);
	
	// engone::log::out << " "<<fileSize << " "<<header.fileSize<<"\n";
	
	// TODO: Check file read errors
	engone::FileRead(file, &header, WAVEHeader::SIZE);

	Assert(header.almostFileSize == fileSize - 8);
	Assert(!strncmp(header.riff, "RIFF", 4));
	Assert(!strncmp(header.wave, "WAVE", 4));
	Assert(!strncmp(header.fmt, "fmt ", 4));
	Assert(!strncmp(header.data, "data", 4));
	Assert(header.lengthOfFormatData == 16);
	Assert(header.audioFormat == 1); // must be 1 which indicates PCM (pulse code modulation)
	Assert(header.byteRate == header.sampleRate * header.bitsPerSample * header.channels / 8);
	Assert(header.blockAlign == header.bitsPerSample * header.channels / 8);
	
	channels = header.channels;
	bps = header.bitsPerSample;
	sampleRate = header.sampleRate;
	size = header.sizeOfData;
	
	i8* data = new i8[size];
	// Assert(size == fileSize - WAVEHeader::SIZE);

	engone::FileRead(file, data, size);

	engone::FileClose(file);
	return data;
}
bool WriteWAVE(const std::string& filename, int& channels, int& sampleRate, int& bps, int& size, i8* data) {
	auto file = engone::FileOpen(filename, engone::FILE_CLEAR_AND_WRITE,  nullptr);
	Assert(file);
	if(!file)
		return false;
	
	WAVEHeader header{};
	
	header.channels = channels;
	header.sampleRate = sampleRate;
	header.bitsPerSample = bps;
	header.sizeOfData = size;
	header.almostFileSize = WAVEHeader::SIZE - 8 + size;
	header.byteRate = header.sampleRate * header.bitsPerSample * header.channels / 8;
	header.blockAlign = header.bitsPerSample * header.channels / 8;
	
	// TODOO: Check write errors
	engone::FileWrite(file, &header, WAVEHeader::SIZE);
	
	engone::FileWrite(file, data, size);
	
	engone::FileClose(file);
	return true;
}