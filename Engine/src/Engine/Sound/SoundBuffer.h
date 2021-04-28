#pragma once
#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>

#define alCall(x) x; if(al_check_error()) __debugbreak();

bool al_check_error();

namespace engine {
	class SoundBuffer {
	public:
		SoundBuffer();
		void Init(const char* path);
		void Delete();

		unsigned int id;
	};
}