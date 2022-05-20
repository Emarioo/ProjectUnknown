#pragma once

#include "Engone/Logger.h"

namespace engone {
	enum class NetEvent : int {
		// client/server connected
		Connect,
		// safe client/server disconnect
		Disconnect,
		// client abruptly failed to connect
		Failed,
		// server was stopped
		Stopped,
	};
	std::string toString(NetEvent t);
	log::logger operator<<(log::logger a, NetEvent b);
}