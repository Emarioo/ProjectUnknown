#pragma once

#define alCall(x) x; if(al_check_error()) __debugbreak();
bool al_check_error();

namespace engone {
	class SoundBuffer {
	public:
		SoundBuffer() = default;
		~SoundBuffer();
		void Init(const char* path);

	private:
		unsigned int id=0;
		bool isInitialized = false;

		friend class SoundSource;
	};
}
