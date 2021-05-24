#pragma once

#include "Sound/SoundStream.h"

namespace engine {
	// returns false if error
	bool InitSound();
	void UninitSound();
	void ListenerPosition(float x, float y, float z);
	void ListenerVelocity(float x, float y, float z);
	/*
	x,y,z front direction
	x1,y1,z1 up direction
	*/
	void ListenerRotation(float x, float y, float z, float x1, float y1, float z1);
}