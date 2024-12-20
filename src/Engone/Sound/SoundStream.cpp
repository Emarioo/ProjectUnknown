#ifdef ENGONE_SOUND

#include "Engone/Sound/SoundStream.h"
#include "Engone/Utilities/Utilities.h"

#define AL_LIBTYPE_STATIC
#include "AL/al.h"
#include "AL/alc.h"
#include "Engone/vendor/Libaudio.h"

namespace engone {
	static ALenum to_al_format(short channels, short samples) {
		bool stereo = (channels > 1);

		switch (samples) {
		case 16:
			if (stereo)
				return AL_FORMAT_STEREO16;
			else
				return AL_FORMAT_MONO16;
		case 8:
			if (stereo)
				return AL_FORMAT_STEREO8;
			else
				return AL_FORMAT_MONO8;
		default:
			return -1;
		}
	}
	SoundStream::~SoundStream() {
		if (isInitialized) {
			delete bufferData;
			source.~SoundSource(); // Todo: Should this be here?
			alCall(alDeleteBuffers(NUM_BUFFERS, &buffer_id[0]));
		}
	}
	void SoundStream::Init(const std::string& path) {
		if (engone::FindFile(path)) {
			int channels, bps;
			bufferData = LoadWAV(path, channels, bufferFreq, bps, bufferSize);
			bufferFormat = to_al_format(channels, bps);

			alCall(alGenBuffers(NUM_BUFFERS, &buffer_id[0]));

			for (std::uint32_t i = 0; i < NUM_BUFFERS; ++i)
				alCall(alBufferData(buffer_id[i], bufferFormat, &bufferData[i * BUFFER_SIZE], BUFFER_SIZE, bufferFreq));

			source.Init();

			alCall(alSourceQueueBuffers(source.id, NUM_BUFFERS, &buffer_id[0]));

			cursor = NUM_BUFFERS * BUFFER_SIZE;
			isInitialized = true;
		} else {
			//TODO: Error code
		}
	}
	void SoundStream::UpdateStream() {
		if (isInitialized) {
			ALint buffersProcessed = 0;
			alCall(alGetSourcei(source.id, AL_BUFFERS_PROCESSED, &buffersProcessed));

			if (buffersProcessed <= 0)
				return;

			while (buffersProcessed--) {
				ALuint buffer;

				alCall(alSourceUnqueueBuffers(source.id, 1, &buffer));

				ALsizei dataSize = BUFFER_SIZE;

				char* data = new char[dataSize];
				std::memset(data, 0, dataSize);

				uint32_t dataSizeToCopy = BUFFER_SIZE;
				if (cursor + BUFFER_SIZE > (uint32_t)bufferSize)
					dataSizeToCopy = (uint32_t)bufferSize - cursor;

				std::memcpy(&data[0], &bufferData[cursor], dataSizeToCopy);
				cursor += dataSizeToCopy;

				if (dataSizeToCopy < BUFFER_SIZE) {
					cursor = 0;
					std::memcpy(&data[dataSizeToCopy], &bufferData[cursor], BUFFER_SIZE - dataSizeToCopy);
					cursor = BUFFER_SIZE - dataSizeToCopy;
				}

				alCall(alBufferData(buffer, bufferFormat, data, BUFFER_SIZE, bufferFreq));
				alCall(alSourceQueueBuffers(source.id, 1, &buffer));

				delete[] data;
			}
		}
	}
}
#endif // ENGONE_SOUND