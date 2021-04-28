#pragma once

#include "SoundBuffer.h"

namespace engine {
	class SoundSource {
	public:
		SoundSource();
		void Init();
		
		void Loop(bool);
		void Gain(float);
		void Pitch(float);
		void Position(float x,float y,float z);
		void Velocity(float x,float y,float z);

		void Delete();
		void Play();
		void Pause();
		void Stop();
		ALint GetState();
		void Bind(SoundBuffer& buffer);

		unsigned int id;
	};
}