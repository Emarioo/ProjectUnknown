#pragma once

#include "Engone/Sound/SoundBuffer.h"

namespace engone {
	class SoundSource {
	private:
		bool isInitialized = false;
	public:
		SoundSource();
		~SoundSource();
		void Init();
		
		void Loop(bool);
		void Gain(float);
		void Pitch(float);
		void Position(float x,float y,float z);
		void Velocity(float x,float y,float z);

		void Bind(SoundBuffer& buffer);
		void Play();
		void Pause();
		void Stop();
		int GetState();

		unsigned int id=0;
	};
}
