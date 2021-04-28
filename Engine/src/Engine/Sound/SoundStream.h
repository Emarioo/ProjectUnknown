#pragma once

#include <iostream>
#include <string>
#include "Libaudio.h"

#include "SoundSource.h"

namespace engine {
	class SoundStream {
	public:
		SoundStream();
		void Init(const std::string& path);
		void UpdateStream();
		void Delete();

		SoundSource source;

	private:
		const std::size_t NUM_BUFFERS = 4;
		const std::size_t BUFFER_SIZE = 65536;// 32kb
		char* bufferData = nullptr;
		ALsizei bufferSize;
		ALsizei bufferFreq;
		ALenum bufferFormat;
		std::size_t cursor=0;
		ALuint buffer_id[4];
	};
}