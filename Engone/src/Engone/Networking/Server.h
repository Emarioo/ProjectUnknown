#pragma once

#include "Engone/Networking/NetEvent.h"
#include "Engone/Networking/Sender.h"
#include "Engone/Networking/MessageBuffer.h"

namespace engone {
	class Server : public Sender {
	public:
		Server();
		~Server();
		// lambda should return true to deny connection, false to accept
		void setOnEvent(std::function<bool(NetEvent, uint32_t)> onEvent);
		// lambda should return true to close connection
		void setOnReceive(std::function<bool(MessageBuffer, uint32_t)> onReceive);

		void start(uint16_t port);
		// called by you
		void disconnect(uint32_t uuid);
		void stop();
		void send(MessageBuffer& msg, uint32_t uuid = -1, bool ignore = false) override;

		friend class Connection;
	private:
		bool keepRunning = false;


		void* m_acceptor; // asio::ip::tcp::acceptor - using void instead so that you don't need to include asio.hpp

		std::unordered_map<uint32_t, Connection*> m_connections;

		std::function<bool(NetEvent, uint32_t)> m_onEvent = nullptr;
		std::function<bool(MessageBuffer, uint32_t)> m_onReceive = nullptr;

		// called from within
		void _disconnect(uint32_t uuid);
		bool shutdown(std::error_code ec);
		void waitForConnection();
	};
}