#pragma once

#include <vendor/Libaudio.h>
#include "Engone/Sound/SoundSource.h"

namespace engone {
	class SoundStream {
	private:
		const std::size_t NUM_BUFFERS = 4;
		const std::size_t BUFFER_SIZE = 65536;// 32kb
		char* bufferData = nullptr;
		int bufferSize=0;
		int bufferFreq=0;
		int32_t bufferFormat=0;
		std::size_t cursor=0;
		uint32_t buffer_id[4];
		bool isInitialized = false;
	public:
		SoundStream();
		~SoundStream();
		void Init(const std::string& path);
		void UpdateStream();

		SoundSource source;
	};
}
