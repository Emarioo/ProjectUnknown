#pragma once

#include "Engone/Networking/MessageBuffer.h"
#include "Engone/Utilities/Utilities.h"

namespace engone {
	// These stats are changed in client and server when messages are sent or receieved.
	// They are incremented when the body message has beem confirmed.
	// Note: the data type is uint64_t. Let's say you want to use snprintf, %u would work fine for %u
	//   but with uint64_t you need "%llu". You could also cast the receivedBytes() to uint32_t.
	struct NetworkStats {
		uint64_t m_receivedBytes = 0, m_sentBytes = 0, m_receivedMessages = 0, m_sentMessages = 0;
	};
	class Sender {
	public:
		Sender(bool isServer) : isServer(isServer) {};

		virtual void send(MessageBuffer& msg, UUID uuid = 0, bool ignore = false) = 0;

		bool isServer = false;

		// amount of recieved bytes.
		// totalBytes will include the whole message (not skipping data sent behind the scenes)
		uint64_t receivedBytes(bool totalBytes = false) const { return m_stats.m_receivedBytes +(totalBytes?sizeof(uint32_t)*receivedMessages():0); }
		// amount of recieved messages
		uint64_t receivedMessages() const { return m_stats.m_receivedMessages; }
		uint64_t sentBytes(bool totalBytes = false) const { return m_stats.m_sentBytes + (totalBytes ? sizeof(uint32_t) * sentMessages() : 0); }
		uint64_t sentMessages() const { return m_stats.m_sentMessages; }
		NetworkStats& getStats() { return m_stats; }

		// maximum bytes that can be sent/recieved. Size does not include header
		// Messages over this limit will be ignored. Implement data streaming to bypass.
		uint32_t getTransferLimit() const { return m_transferLimit; }
		void setTransferLimit(uint32_t limit) { m_transferLimit = limit; }

	protected:
		NetworkStats m_stats;
		// default limit of 10 Megabytes.
		uint32_t m_transferLimit=10000000;

	};
}