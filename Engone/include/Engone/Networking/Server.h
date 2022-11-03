#pragma once

#include "Engone/Networking/NetEvent.h"
#include "Engone/Networking/Sender.h"
#include "Engone/Networking/MessageBuffer.h"

//#include <functional>
//#include <unordered_map>
//#include <thread>

#ifdef ENGONE_TRACKER
#include "Engone/Utilities/Tracker.h"
#endif

namespace engone {
	class Connection;
	class Server : public Sender {
	public:
		Server() : Sender(true, "127.0.0.1") {}
		~Server();

		// set lambdas before starting.
		// returns false if port was invalid.
		// returns true if server started or already running
		bool start(const std::string& port);
		// close one connection
		void disconnect(UUID uuid);
		// stop server and all connections.
		// does not wait for connections to close. A silent stop.
		void stop();
		void send(MessageBuffer& msg, UUID uuid = 0, bool excludeUUID = false, bool synchronous = false) override;

		uint32_t getConnectionCount() const { return m_connections.size(); }

		inline bool isRunning() { return keepRunning; }

		// will wait for everything to terminate unlike stop
		void cleanup();

#ifdef ENGONE_TRACKER
		static TrackerId trackerId;
#endif
	private:
		bool keepRunning = false;

		std::unordered_map<UUID, Connection*> m_connections;

		// void* should be SOCKET, but isn't so that you don't need to include in this header.
		void* m_socket=nullptr;
		std::thread m_acceptThread;
		std::thread m_workerThread;

		std::mutex m_mutex;
		std::thread::id m_mutexOwner;
		int mutexDepth = 0;
		void lock();
		void unlock();

		friend class Connection;
	};
}