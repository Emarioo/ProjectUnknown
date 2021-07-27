#include "SoundSource.h";

namespace engine {
	SoundSource::SoundSource() {

	}
	SoundSource::~SoundSource() {
		if (isInitialized)
			alCall(alDeleteSources(1, &id));
	}
	void SoundSource::Init() {
		alCall(alGenSources(1, &id));
		isInitialized = true;
		//TEST_ERROR("source generation");
	}
	void SoundSource::Loop(bool b) {
		if (isInitialized)
			alCall(alSourcei(id, AL_LOOPING, b));
		//TEST_ERROR("source looping");
	}
	void SoundSource::Gain(float f) {
		if (isInitialized)
			alCall(alSourcef(id, AL_GAIN, f));
	}
	void SoundSource::Pitch(float f) {
		if (isInitialized)
			alCall(alSourcef(id, AL_PITCH, f));
		//TEST_ERROR("source pitch");
	}
	void SoundSource::Position(float x, float y, float z) {
		if (isInitialized)
			alCall(alSource3f(id, AL_POSITION, x, y, z));
	}
	void SoundSource::Velocity(float x, float y, float z) {
		if (isInitialized)
			alCall(alSource3f(id, AL_VELOCITY, x, y, z));
		//TEST_ERROR("source velocity");
	}
	void SoundSource::Bind(SoundBuffer& buffer) {
		if(isInitialized)
			alCall(alSourcei(id, AL_BUFFER, buffer.id));
		//TEST_ERROR("buffer binding");
	}
	void SoundSource::Play() {
		if (isInitialized)
			alCall(alSourcePlay(id));
		//TEST_ERROR("source playing");
	}
	void SoundSource::Pause() {
		if (isInitialized)
			alCall(alSourcePause(id));
		//TEST_ERROR("source pause");
	}
	void SoundSource::Stop() {
		if (isInitialized)
			alCall(alSourceStop(id));
		//TEST_ERROR("source stop");
	}
	ALint SoundSource::GetState() {
		if (isInitialized) {
			ALint state;
			alCall(alGetSourcei(id, AL_SOURCE_STATE, &state));
			return state;
		}
		return AL_STOPPED;
	}
}