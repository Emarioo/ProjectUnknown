#ifdef ENGONE_SOUND

#include "Engone/Sound/SoundSource.h"

#define AL_LIBTYPE_STATIC
#include "AL/al.h"
#include "AL/alc.h"

namespace engone {
	SoundSource::~SoundSource() {
		if (!valid()) return;
		alCall(alDeleteSources(1, &id));
	}
	void SoundSource::Init() {
		alCall(alGenSources(1, &id));
		//isInitialized = true;
		//TEST_ERROR("source generation");
	}
	void SoundSource::Loop(bool b) {
		if (!valid()) return;
		alCall(alSourcei(id, AL_LOOPING, b));
		//TEST_ERROR("source looping");
	}
	void SoundSource::Gain(float f) {
		if (!valid()) return;
		alCall(alSourcef(id, AL_GAIN, f));
	}
	void SoundSource::Pitch(float f) {
		if (!valid()) return;
		alCall(alSourcef(id, AL_PITCH, f));
		//TEST_ERROR("source pitch");
	}
	void SoundSource::Position(float x, float y, float z) {
		if (!valid()) return;
		alCall(alSource3f(id, AL_POSITION, x, y, z));
	}
	void SoundSource::Velocity(float x, float y, float z) {
		if (!valid()) return;
		alCall(alSource3f(id, AL_VELOCITY, x, y, z));
		//TEST_ERROR("source velocity");
	}
	void SoundSource::Bind(SoundBuffer& buffer) {
		if (!valid()) return;
		alCall(alSourcei(id, AL_BUFFER, buffer.id));
		//TEST_ERROR("buffer binding");
	}
	void SoundSource::Play() {
		if (!valid()) return;
			alCall(alSourcePlay(id));
		//TEST_ERROR("source playing");
	}
	void SoundSource::Pause() {
		if (!valid()) return;
			alCall(alSourcePause(id));
		//TEST_ERROR("source pause");
	}
	void SoundSource::Stop() {
		if (!valid()) return;
			alCall(alSourceStop(id));
		//TEST_ERROR("source stop");
	}
	int SoundSource::GetState() {
		if (!valid()) return AL_STOPPED;

		ALint state;
		alCall(alGetSourcei(id, AL_SOURCE_STATE, &state));
		return state;
	}
}
#endif  // ENGONE_SOUND