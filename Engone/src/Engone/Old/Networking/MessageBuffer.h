#pragma once

#include "Engone/Utilities/Tracker.h"

namespace engone {
	 //A message consists of the size of the body and then the body which is raw data.
	 //4.29 Gigabytes is the maximum of how many bytes you can send.
	 //Send multiple messages if you need to send more data.
	class MessageBuffer {
	public:
		// msgType can be an enum which describes what type of message you are sending.
		// size is optional, if you know the size of the body this will would prevent
		// unnecessary reallocations of the internal buffer.

		//static MessageBuffer* Create(int msgType, uint32_t size = 0);
		MessageBuffer(int msgType, uint32_t size = 0);
		MessageBuffer() = default;
		~MessageBuffer();
		// size of body
		uint32_t size() const {
			if (m_data) {
				return *((uint32_t*)m_data);
			}
			return 0;
		}

		// count will be pushed. You should then write data into the returned pointer.
		char* pushBuffer(uint32_t count);
		template<class T>
		void push(T* in, int count = 1) {
			if (m_dataSize < sizeof(uint32_t)+ size() + sizeof(T) * count) {
				if (!resize((m_dataSize + sizeof(T) * count) * 2)) {
					// resize will give error
					return;
				}
			}
			uint32_t head = sizeof(uint32_t)+size();
			memcpy_s(m_data + head, m_dataSize - head, in, sizeof(T) * count);
			*((uint32_t*)m_data) += sizeof(T) * count;
		}
		template<class T>
		void push(const T in) {
			if (m_dataSize < sizeof(uint32_t)+size() + sizeof(T)) {
				if (!resize((m_dataSize + sizeof(T)) * 2)) {
					// resize will give error
					return;
				}
			}
			uint32_t head = sizeof(uint32_t)+ size();
			memcpy_s(m_data + head, m_dataSize -head, &in, sizeof(T));
			*((uint32_t*)m_data) += sizeof(T);
		}
		void push(const std::string& in);
		void push(const char* in);

		// size of buffer will be written into pointer
		char* pullBuffer(uint32_t* count);
		template<class T>
		void pull(T* out, uint32_t count = 1) {
			if (sizeof(T) * count > size() - m_readHead) {
				// fishy
				std::cout << "Corrupted network message, did you turn a string into a pointer?\n";
				return;
			}
			std::memcpy(out, m_data + sizeof(uint32_t)+ m_readHead, sizeof(T) * count);
			m_readHead += sizeof(T) * count;
		}
		void pull(std::string& out);

		void flush();

		// The internal buffer is grouped with MessageBuffer
		static TrackerId trackerId;
	private:
		char* m_data = nullptr;
		uint32_t m_dataSize = 0; // including size of header
		uint32_t m_readHead=0;
		bool noDelete = false; // used when receiving
		//uint32_t sharings = 0;

		// size should exclude size of header
		bool resize(uint32_t size);

		friend class Connection;
		friend class Client;
		friend class Server;
	};
}