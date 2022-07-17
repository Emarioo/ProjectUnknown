#pragma once

namespace engone {
	class MessageBuffer {
	public:
		MessageBuffer(int header, int size = 8);
		MessageBuffer() = default;
		~MessageBuffer();
		uint32_t size();
		char* pushBuffer(uint32_t count);
		template<class T>
		void push(T* in, int count = 1) {
			if (m_data) {
				uint32_t head = size();
				if (m_maxSize < head + sizeof(T) * count) {
					if (!resize((m_maxSize + sizeof(T) * count) * 2)) {
						// failed
						return;
					}
				}
				std::memcpy(m_data + head, in, sizeof(T) * count);
				*((uint32_t*)m_data) += sizeof(T) * count;
			}
		}
		template<class T>
		void push(const T in) {
			if (m_data) {
				uint32_t head = size();
				if (m_maxSize < head + sizeof(T)) {
					if (!resize((m_maxSize + sizeof(T)) * 2)) {
						// failed
						return;
					}
				}
				std::memcpy(m_data + head, &in, sizeof(T));
				*((uint32_t*)m_data) += sizeof(T);
			}
		}
		void push(const std::string& in);
		void push(const char* in);

		char* pullBuffer(uint32_t* count);
		template<class T>
		void pull(T* out, uint32_t count = 1) {
			if (m_data) {
				if (sizeof(T) * count > size() - m_readHead) {
					// fishy
					std::cout << "Corrupted network message, did you turn a string into a pointer?\n";
					return;
				}
				std::memcpy(out, m_data + m_readHead, sizeof(T) * count);
				m_readHead += sizeof(T) * count;
			}
		}
		void pull(std::string& out);

		void flush();
	private:
		static const uint32_t HEAD_SIZE = 4;
		char* m_data = nullptr;
		uint32_t m_maxSize = 0;
		uint32_t m_readHead = HEAD_SIZE; // why start at buffer size
		bool noDelete = false;

		bool resize(uint32_t size);

		friend class Connection;
		friend class Client;
		friend class Server;
	};
}