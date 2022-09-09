#pragma once

#include "Engone/Networking/MessageBuffer.h"
#include "Engone/Utilities/RandomUtility.h"

//#include <functional>

namespace engone {
	// These stats are changed in client and server when messages are sent or receieved.
	// They are incremented when the body message has beem confirmed.
	// Note: the data type is uint64_t. Let's say you want to use snprintf, %u would work fine for %u
	//   but with uint64_t you need "%llu". You could also cast the receivedBytes() to uint32_t.
	struct NetworkStats {
		uint64_t m_receivedBytes = 0, m_sentBytes = 0, m_receivedMessages = 0, m_sentMessages = 0;
		
		uint64_t receivedBytes() const { return m_receivedBytes; }
		uint64_t receivedMessages() const { return m_receivedMessages; }
		uint64_t sentBytes() const { return m_sentBytes; }
		uint64_t sentMessages() const { return m_sentMessages; }

		// this is the bytes given to winsock m_sentBytes + (totalBytes ? sizeof(uint32_t) * sentMessages() : 0);
	};
//#ifndef ENGONE_RANDOM
	//typedef uint64_t UUID; // replacement for UUID struct from engone utilities.
//#endif
	class Sender {
	public:
		Sender(bool isServer) : m_isServer(isServer) {};

		// lambda should return false to deny connection, true to accept.
		inline void setOnEvent(std::function<bool(NetEvent,UUID)> onEvent) { m_onEvent = onEvent; }
		// lambda should return false to close connection, true to keep connection going.
		inline void setOnReceive(std::function<bool(MessageBuffer&,UUID)> onReceive) { m_onReceive = onReceive; }

		// synchronous as true will block current thread until message has been sent.
		// False will use a write thread to send messages.
		virtual void send(MessageBuffer& msg, UUID uuid = 0, bool excludeUUID = false, bool synchronous=false) = 0;

		// maximum bytes that can be sent/recieved. Size does not include header
		// Messages over this limit will be ignored. Implement data streaming to bypass.
		uint32_t getTransferLimit() const { return m_transferLimit; }
		void setTransferLimit(uint32_t limit) { m_transferLimit = limit; }

		NetworkStats& getStats() { return m_stats; }

		inline bool isServer() const { return m_isServer; }

	protected:
		bool m_isServer = false;

		std::function<bool(NetEvent,UUID)> m_onEvent = nullptr;
		std::function<bool(MessageBuffer&,UUID)> m_onReceive = nullptr;

		// default limit based on size of network stack which recv can use. Not sure how correct this value is.
		// But I would assume a normal computer should be able to handle it without issue.
		uint32_t m_transferLimit=256000;
		NetworkStats m_stats;

		friend class Connection;

	};
}