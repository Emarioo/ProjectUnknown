#pragma once

#include "Engone/Logger.h"

namespace engone {
	enum class NetEvent : int {
		// client/server connected
		Connect,
		// safe client/server disconnect
		Disconnect,
		// for client, abruptly failed to connect
		Failed,
		// for server when stopped
		Stopped,
	};
	std::string toString(NetEvent t);
	Logger& operator<<(Logger& log, NetEvent value);
}