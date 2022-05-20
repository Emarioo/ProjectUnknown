#pragma once

#include "Engone/Networking/NetEvent.h"
#include "Engone/Networking/Sender.h"
#include "Engone/Networking/MessageBuffer.h"

namespace engone {
	class Connection;
	class Client : public Sender {
	public:
		Client() = default;
		~Client();
		// lambda should return true to deny connection, false to accept
		void setOnEvent(std::function<bool(NetEvent)> onEvent);
		// lambda should return true to close connection
		void setOnReceive(std::function<bool(MessageBuffer)> onReceive);
		/*
		set on events before connecting
		*/
		void connect(const std::string& ip, uint16_t port);
		// Called by you, will call close on connection.
		void disconnect();
		// uuid and ignore is not relevant for client
		void send(MessageBuffer& msg, uint32_t uuid = -1, bool ignore = false) override;

		friend class Connection;
	private:
		bool keepRunning = false;

		// When the connection's read and write listeners failed due to connection closing.
		// This will then be called to clean data.
		void _disconnect();
		// Will only work when connection doesn't have any work
		void clean();

		Connection* m_connection = nullptr;

		std::function<bool(NetEvent)> m_onEvent = nullptr;
		std::function<bool(MessageBuffer)> m_onReceive = nullptr;
	};
}