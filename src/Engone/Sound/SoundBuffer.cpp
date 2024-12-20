#ifdef ENGONE_SOUND

#include "Engone/Sound/SoundBuffer.h"

#include "Engone/Utilities/Utilities.h"

#define AL_LIBTYPE_STATIC
#include "AL/al.h"
#include "AL/alc.h"
#include "Engone/vendor/Libaudio.h"

bool al_check_error() {
	// Todo: Check if there is an active context?
	ALenum err = alGetError();
	if (err != AL_NO_ERROR) {
		const ALchar* str = alGetString(err);
		if (str) {
			engone::log::out << "Al Err: " << err << " " << str << "\n";
			return true;
		} else {
			engone::log::out << "Al Err: " << err << " null string, no active context?" << "\n";
		}
	}
	return false;
}

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
	SoundBuffer::~SoundBuffer() {
		if (!valid()) return;
		alCall(alDeleteBuffers(1, &id));
	}
	void SoundBuffer::Init(const char* path) {
		if (engone::FindFile(path)) {
			//TEST_ERROR("buffer generation");
			ALsizei size, freq;
			int channels, bps;
			char* bufferData = LoadWAV(path, channels, freq, bps, size);
			ALenum format = to_al_format(channels, bps);

			alCall(alGenBuffers(1, &id));

			alCall(alBufferData(id, format, bufferData, size, freq));

			delete bufferData;
		} else {
			log::out << log::RED <<"SoundBuffer : "<< path << " not found\n";
			// TODO: Error code
		}
	}
}
#endif // ENGONE_SOUND