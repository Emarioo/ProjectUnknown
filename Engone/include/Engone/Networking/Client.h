#pragma once

#include "Engone/Networking/NetEvent.h"
#include "Engone/Networking/Sender.h"
#include "Engone/Networking/MessageBuffer.h"

//#include <functional>
//#include <thread>

#ifdef ENGONE_TRACKER
#include "Engone/Utilities/Tracker.h"
#endif
namespace engone {
	class Connection;
	class Client : public Sender {
	public:
		// Constructor does nothing
		Client() : Sender(false) {}
		~Client();
		/*
		set lambdas before connecting.
		returns false if ip or port were invalid.
		returns true if client already is connected/connecting or has started connecting.
		*/
		bool start(const std::string& ip, const std::string& port);
		// will disconnect but won't wait for connection and threads to close.
		void stop();

		// uuid and excludeUUID is not relevant for client
		void send(MessageBuffer& msg, UUID uuid = 0, bool excludeUUID = false,bool synchronous = false) override;

		inline bool isRunning() { return keepRunning; }

		// waits for everything to terminate unlike stop.
		void cleanup();
#ifdef ENGONE_TRACKER
		static TrackerId trackerId;
#endif
	private:
		bool keepRunning = false;
		Connection* m_connection = nullptr;

		std::thread m_workerThread; // connect thread

		std::mutex m_mutex; // general mutex
		int mutexDepth = 0;
		std::thread::id m_mutexOwner;
		void lock();
		void unlock();

		friend class Connection;
	};
}