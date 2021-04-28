#include "SoundSource.h";

namespace engine {
	SoundSource::SoundSource() {

	}
	void SoundSource::Init() {
		alCall(alGenSources(1, &id));
		//TEST_ERROR("source generation");
	}
	void SoundSource::Loop(bool b) {
		alCall(alSourcei(id, AL_LOOPING, b));
		//TEST_ERROR("source looping");
	}
	void SoundSource::Gain(float f) {
		alCall(alSourcef(id, AL_GAIN, f));
	}
	void SoundSource::Pitch(float f) {
		alCall(alSourcef(id, AL_PITCH, f));
		//TEST_ERROR("source pitch");
	}
	void SoundSource::Position(float x, float y, float z) {
		alCall(alSource3f(id, AL_POSITION, x, y, z));
	}
	void SoundSource::Velocity(float x, float y, float z) {
		alCall(alSource3f(id, AL_VELOCITY, x, y, z));
		//TEST_ERROR("source velocity");
	}
	void SoundSource::Delete() {
		alCall(alDeleteSources(1, &id));
	}
	void SoundSource::Play() {
		alCall(alSourcePlay(id));
		//TEST_ERROR("source playing");
	}
	void SoundSource::Pause() {
		alCall(alSourcePause(id));
		//TEST_ERROR("source pause");
	}
	void SoundSource::Stop() {
		alCall(alSourceStop(id));
		//TEST_ERROR("source stop");
	}
	ALint SoundSource::GetState() {
		ALint state;
		alCall(alGetSourcei(id, AL_SOURCE_STATE, &state));
		return state;
	}
	void SoundSource::Bind(SoundBuffer& buffer) {
		alCall(alSourcei(id, AL_BUFFER, buffer.id));
		//TEST_ERROR("buffer binding");
	}
}