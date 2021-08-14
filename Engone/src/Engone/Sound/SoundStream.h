#pragma once

#include "Libaudio.h"

#include "SoundSource.h"

namespace engine {
	class SoundStream {
	private:
		const std::size_t NUM_BUFFERS = 4;
		const std::size_t BUFFER_SIZE = 65536;// 32kb
		char* bufferData = nullptr;
		ALsizei bufferSize=0;
		ALsizei bufferFreq=0;
		ALenum bufferFormat=0;
		std::size_t cursor=0;
		ALuint buffer_id[4];
		bool isInitialized = false;
	public:
		SoundStream();
		~SoundStream();
		void Init(const std::string& path);
		void UpdateStream();

		SoundSource source;
	};
}