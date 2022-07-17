#pragma once

#include "Engone/Networking/Server.h"
#include "Engone/Networking/Client.h"

namespace engone {
	// Will initialize itself if you make a server or client
	void InitIOContext();
	// Engone will call this when destroyed.
	void DestroyIOContext();
}