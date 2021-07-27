#include "SoundBuffer.h"

#include "Handlers/FileHandler.h"

#include "Libaudio.h"

bool al_check_error() {
	ALenum err = alGetError();
	if (err != AL_NO_ERROR) {
		std::cout << alGetString(err) << std::endl;
		return true;
	}
	return false;
}

namespace engine {
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

	SoundBuffer::SoundBuffer() {

	}
	SoundBuffer::~SoundBuffer() {
		if (isInitialized)
			alCall(alDeleteBuffers(1, &id));
	}
	void SoundBuffer::Init(const char* path) {
		if (engine::FileExist(path)) {
			//TEST_ERROR("buffer generation");
			ALsizei size, freq;
			int channels, bps;
			char* bufferData = LoadWAV(path, channels, freq, bps, size);
			ALenum format = to_al_format(channels, bps);

			alCall(alGenBuffers(1, &id));

			alCall(alBufferData(id, format, bufferData, size, freq));

			delete bufferData;
			isInitialized = true;
		} else {
			// TODO: Error code
		}
	}
}