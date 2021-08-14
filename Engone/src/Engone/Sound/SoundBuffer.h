#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#define alCall(x) x; if(al_check_error()) __debugbreak();

bool al_check_error();

namespace engine {
	class SoundBuffer {
	private:
		bool isInitialized = false;
	public:
		SoundBuffer();
		~SoundBuffer();
		void Init(const char* path);

		unsigned int id=0;
	};
}