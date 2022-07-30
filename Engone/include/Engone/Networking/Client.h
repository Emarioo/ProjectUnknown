#pragma once

#include "Engone/Networking/NetEvent.h"
#include "Engone/Networking/Sender.h"
#include "Engone/Networking/MessageBuffer.h"

namespace engone {
	class Connection;
	class Client : public Sender {
	public:
		// Constructor does nothing
		Client();
		~Client();
		// lambda should return false to deny connection, true to accept
		void setOnEvent(std::function<bool(NetEvent)> onEvent);
		// lambda should return false to close connection
		void setOnReceive(std::function<bool(MessageBuffer)> onReceive);
		/*
		set on events before connecting.
		returns false if ip or port were invalid.
		returns true if client already is connected/connecting or has started connecting.
		*/
		bool connect(const std::string& ip, const std::string& port);
		// Called by you, will call close on connection.
		void disconnect();
		// uuid and ignore is not relevant for client
		void send(MessageBuffer& msg, UUID uuid = 0, bool ignore = false) override;

		inline bool isRunning() { return keepRunning; }

		static TrackerId trackerId;
	private:
		bool keepRunning = false;
		Connection* m_connection = nullptr;
		std::function<bool(NetEvent)> m_onEvent = nullptr;
		std::function<bool(MessageBuffer)> m_onReceive = nullptr;

		// When the connection's read and write listeners failed due to connection closing.
		// This will then be called to clean data.
		void _disconnect();
		// Will only work when connection doesn't have any work
		void clean();


		friend class Connection;
	};
}