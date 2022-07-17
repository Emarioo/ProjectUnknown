#pragma once

#include "Engone/Networking/MessageBuffer.h"

namespace engone {
	class Sender {
	public:
		Sender() = default;

		virtual void send(MessageBuffer& msg, uint32_t uuid = -1, bool ignore = false) = 0;
	};
}