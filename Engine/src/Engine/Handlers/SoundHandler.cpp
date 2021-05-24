#include "SoundHandler.h"

#include <iostream>

namespace engine {

	// Add Debug option? sound_device_list
	void list_audio_devices(const ALCchar* devices)
	{
		const ALCchar* device = devices, * next = devices + 1;
		size_t len = 0;

		std::cout << "Devices list:" << std::endl;;
		while (device && *device != '\0' && next && *next != '\0') {
			std::cout << " " << device << std::endl;;
			len = strlen(device);
			device += (len + 1);
			next += (len + 2);
		}
	}
	// Switch to vec3
	void ListenerPosition(float x, float y, float z) {
		alCall(alListener3f(AL_POSITION, 0, 0, 0));
	}
	void ListenerVelocity(float x, float y, float z) {
		alCall(alListener3f(AL_VELOCITY, 0, 0, 0));
	}
	void ListenerRotation(float x, float y, float z, float x1, float y1, float z1) {
		float f[]{ x,y,z,x1,y1,z1 };
		alCall(alListenerfv(AL_ORIENTATION, f));
	}
	ALCdevice* device;
	ALCcontext* context;
	bool InitSound() {
		ALvoid* data;
		ALint source_state;

		ALboolean enumeration = alCall(alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"));
		if (enumeration == AL_FALSE)

			std::cout << "Enumeration extension not available" << std::endl;

		//list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
		std::cout << alcGetString(NULL, ALC_DEVICE_SPECIFIER) << std::endl;

		const ALCchar* defaultDeviceName = alCall(alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER));

		device = alCall(alcOpenDevice(defaultDeviceName));
		if (!device) {
			std::cout << "Unable to open default device" << std::endl;
			return -1;
		}
		// Add Debug option? sound_device_list
		const ALchar* spec = alCall(alcGetString(device, ALC_DEVICE_SPECIFIER));
		std::cout << "Device: " << spec << std::endl;

		context = alCall(alcCreateContext(device, NULL));
		bool b = alCall(alcMakeContextCurrent(context));
		if (!b) {
			std::cout << "Failed to make default context" << std::endl;
			return false;
		}
		//TEST_ERROR("make default context");

		ListenerPosition(0, 0, 0);
		ListenerVelocity(0, 0, 0);
		ListenerRotation(0, 0, 1, 0, 1, 0);
	}
	void UninitSound() {
		device = alCall(alcGetContextsDevice(context));
		alCall(alcMakeContextCurrent(NULL));
		alCall(alcDestroyContext(context));
		alCall(alcCloseDevice(device));
	}
}