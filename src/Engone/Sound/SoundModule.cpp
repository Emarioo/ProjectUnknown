#ifdef ENGONE_SOUND
#include "Engone/SoundModule.h"

#include "Engone/Logger.h"

#define AL_LIBTYPE_STATIC
#include "AL/al.h"
#include "AL/alc.h"

namespace engone {

	// Add Debug option? sound_device_list
	//void list_audio_devices(const char* devices)
	//{
	//	const ALCchar* device = devices, * next = devices + 1;
	//	uint32_t len = 0;

	//	std::cout << "Devices list:" << std::endl;;
	//	while (device && *device != '\0' && next && *next != '\0') {
	//		std::cout << " " << device << std::endl;;
	//		len = strlen(device);
	//		device += (len + 1);
	//		next += (len + 2);
	//	}
	//}
	std::string ListDefaultDevice() {
		const char* str = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
		return str;
	}
	std::vector<std::string> ListDevices() {
		std::vector<std::string> out;

		const char* str = nullptr;

		// Check if enumeration is available
		if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT")) {
			str = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
		} else {
			log::out << log::YELLOW << "ALC_ENUMERATE_ALL_EXT not available\n";
		}
		if (!str&&alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT")) {
			str = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
		} else if (!str){
			log::out << log::YELLOW << "ALC_ENUMERATION_EXT not available\n";
		}

		if (!str) {
			// Use default device
			out.push_back(ListDefaultDevice());
		} else {
			// Extract device names
			while (true) {
				int len = strlen(str);
				if (len != 0)
					out.push_back(std::string(str));
				str += len + 1;
				if (*str == '\0')
					break;
			}
		}

		// Log devices
		log::out << "Devices:\n";
		for (std::string& s : out)
			log::out << " " << s << "\n";

		return out;
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
		//ALvoid* data;
		//ALint source_state;

		ListDevices();

		std::string defaultDeviceName = ListDefaultDevice();

		device = alcOpenDevice(defaultDeviceName.c_str());
		if (!device) {
			std::cout << "Unable to open default device" << std::endl;
			return false;
		}
		
		context = alcCreateContext(device, NULL);
		bool b = alCall(alcMakeContextCurrent(context));
		if (!b) {
			std::cout << "Failed to make default context" << std::endl;
			return false;
		}

		ListenerPosition(0, 0, 0);
		ListenerVelocity(0, 0, 0);
		ListenerRotation(0, 0, 1, 0, 1, 0);
		return true;
	}
	void UninitSound() {
		device = alCall(alcGetContextsDevice(context));
		alCall(alcMakeContextCurrent(NULL));
		alCall(alcDestroyContext(context));
		alCall(alcCloseDevice(device));
	}
}
#endif  // ENGONE_SOUND