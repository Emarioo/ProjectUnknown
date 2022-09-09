#pragma once

#include "Engone/Networking/NetEvent.h"
#include "Engone/Networking/Sender.h"
#include "Engone/Networking/MessageBuffer.h"

namespace engone {
	class Connection;
	class Server : public Sender {
	public:
		// Constructor sets up IO Context
		Server();
		~Server();
		// lambda should return false to deny connection, true to accept
		void setOnEvent(std::function<bool(NetEvent, UUID)> onEvent);
		// lambda should return false to close connection
		void setOnReceive(std::function<bool(MessageBuffer, UUID)> onReceive);

		// returns false if port was invalid.
		// returns true if server started or already running
		bool start(const std::string& port);
		// close one connection
		void disconnect(UUID uuid);
		// stop server and all connections
		void stop();
		void send(MessageBuffer& msg, UUID uuid = 0, bool ignore = false) override;

		uint32_t getConnectionCount() const { return m_connections.size(); }

		inline bool isRunning() { return keepRunning; }

		static TrackerId trackerId;
	private:
		bool keepRunning = false;
		//asio::ip::tcp::acceptor test;
		void* m_acceptor=nullptr; // asio::ip::tcp::acceptor - using void instead so that you don't need to include asio.hpp
		//void* m_acceptor; // asio::ip::tcp::acceptor - using void instead so that you don't need to include asio.hpp

		std::unordered_map<UUID, Connection*> m_connections;
		std::function<bool(NetEvent, UUID)> m_onEvent = nullptr;
		std::function<bool(MessageBuffer, UUID)> m_onReceive = nullptr;

		// called from within
		void _disconnect(UUID uuid);
		bool shutdown(std::error_code ec);
		void waitForConnection();

		friend class Connection;
	};
}