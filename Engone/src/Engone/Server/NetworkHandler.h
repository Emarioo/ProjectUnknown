#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>

#ifndef MAX_MESSAGE_BUFFER
	#define MAX_MESSAGE_BUFFER 100
#endif

#include "../Logger.h"

namespace engone {

	asio::io_context& GetIOContext();
	void InitIOContext();

	uint32_t GenerateUUID();

	enum class NetEvent : int {
		// client/server connected
		Connect,
		// safe client/server disconnect
		Disconnect,
		// client abruptly failed to connect
		Failed,
		// server was stopped
		Stopped,
	};
	std::string toString(NetEvent t);
	log::logger operator<<(log::logger a, NetEvent b);

	class MessageBuffer {
	private:
		static const int HEAD_SIZE = 4;
		char* m_data = nullptr;
		uint32_t m_maxSize = 0;
		uint32_t m_readHead = HEAD_SIZE; // why start at buffer size
		bool noDelete = false;

		friend class Connection;
		friend class Client;
		friend class Server;

		bool resize(int size) {
			if (size <= m_maxSize)
				size = 2 * m_maxSize;

			char* data;
			if (!m_data) {
				data = (char*)malloc(size);
				if (data)
					*((int*)data) = 4; // 4 bytes for storing size of buffer
			} else {
				data = (char*)realloc(m_data, size);
			}
			if (data) {
				m_data = data;
				m_maxSize = size;
				return true;
			} else {
				// failed
				return false;
			}
		}
	public:
		MessageBuffer(int header, int size = 8);
		MessageBuffer();
		~MessageBuffer();
		void print() {
			int len = size();
			for (int i = 0; i < len; i++) {
				log::out << m_data[i];
			}
			log::out << "\n";
		}
		int size() {
			if (m_data) {
				return *((int*)m_data);
			}
			return 0;
		}
		char* pushBuffer(int count) {
			push(&count);
			if (count == 0)
				return nullptr;
			if (m_data) {
				int head = size();
				if (m_maxSize < head + count) {
					if (!resize((m_maxSize + count) * 2)) {
						// failed
						return nullptr;
					}
				}
				*((int*)m_data) += count;
				return m_data + head;
			}
		}
		template<class T>
		void push(T* in, int count = 1) {
			if (m_data) {
				int head = size();
				if (m_maxSize < head + sizeof(T) * count) {
					if (!resize((m_maxSize + sizeof(T) * count) * 2)) {
						// failed
						return;
					}
				}
				std::memcpy(m_data + head, in, sizeof(T) * count);
				*((int*)m_data) += sizeof(T) * count;
			}
		}
		template<class T>
		void push(const T in) {
			if (m_data) {
				int head = size();
				if (m_maxSize < head + sizeof(T)) {
					if (!resize((m_maxSize + sizeof(T)) * 2)) {
						// failed
						return;
					}
				}
				std::memcpy(m_data + head, &in, sizeof(T));
				*((int*)m_data) += sizeof(T);
			}
		}
		void push(const std::string& in) {
			if (m_data) {
				int head = size();
				if (m_maxSize < head + in.length() + 1) {
					if (!resize((m_maxSize + in.length() + 1) * 2)) {
						// failed
						return;
					}
				}
				std::memcpy(m_data + head, in.data(), in.length() + 1);
				*((int*)m_data) += in.length() + 1;
			}
		}
		void push(const char* in) {
			if (m_data) {
				int len = std::strlen(in) + 1;
				int head = size();
				if (m_maxSize < head + len) {
					if (!resize((m_maxSize + len) * 2)) {
						// failed
						return;
					}
				}
				std::memcpy(m_data + head, in, len);
				*((int*)m_data) += len;
			}
		}
		char* pullBuffer(int* count) {
			pull(count);
			if (*count == 0)
				return nullptr;
			if (m_data) {
				if (*count > size() - m_readHead) {
					// fishy
					std::cout << "Corrupted network message, did you turn a string into a pointer?\n";
					return nullptr;
				}
				char* out = m_data + m_readHead;
				m_readHead += *count;
				return out;
			}
			return nullptr;
		}

		template<class T>
		void pull(T* out, int count = 1) {
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
		void pull(std::string& out) {
			if (m_data) {
				int length = strlen(m_data + m_readHead) + 1;
				if (length > size() - m_readHead) {
					// fishy
					std::cout << "Corrupted network message\n";
					return;
				}
				out.append(std::string_view(m_data + m_readHead));
				m_readHead += length;
			}
		}
		void flush() {
			*((int*)m_data) = HEAD_SIZE; // first for bytes for the size of the buffer
			m_readHead = HEAD_SIZE;
		}
	};
	class Server;
	class Client;
	class Connection {
	public:
		Connection(asio::io_context& io_context) : m_socket(io_context) {}
		Connection(asio::ip::tcp::socket socket) : m_socket(std::move(socket)) {}
		// delete own buffer, unshare all messages
		~Connection() {
			//close();
			if (m_buffer) {
				delete m_buffer;
			}
			m_outMessages.clear();
		}
		void close() {
			if (m_socket.is_open()) {
				try {
					if (hasWork()) {
						m_socket.shutdown(asio::ip::tcp::socket::shutdown_both);
					}
					m_socket.close();
				} catch (asio::system_error se) {
					log::out << log::RED << se.what() << "\n";
				}
			}
		}

		void send(std::shared_ptr<MessageBuffer> msg) {
			if (!m_socket.is_open())
				return;

			asio::post(GetIOContext(), [this, msg]() {

				m_outMessages.push_back(msg);
				if (!writingHead && !writingBody && (readingHead || readingBody)) {
					if (m_outMessages.size() == 1) {
						writeHead();
					}
				}
			});
		}
		uint32_t m_uuid = 0;
	private:
		friend class Client;
		friend class Server;

		asio::ip::tcp::socket m_socket;

		MessageBuffer* m_buffer = nullptr;

		Server* m_server = nullptr;
		Client* m_client = nullptr;

		bool readingHead = false, readingBody = false, writingHead = false, writingBody = false;
		bool hasWork() {
			//std::cout << m_uuid << " "<<readingHead << " " <<readingBody << " " << writingHead << " " << writingBody << "\n";
			return (readingHead + readingBody + writingHead + writingBody) != 0;
		}
		bool forcedShutdown(asio::error_code ec);
		void readHead();
		void readBody();
		void writeHead();
		void writeBody();

		std::vector<std::shared_ptr<MessageBuffer>> m_outMessages;
	};
	class Sender {
	public:
		Sender() = default;

		virtual void send(MessageBuffer& msg, uint32_t uuid = -1, bool ignore = false) = 0;
	};
	class Server : public Sender {
	public:
		Server() : m_acceptor(GetIOContext()) {}
		~Server() {
			stop();
		}
		// lambda should return true to deny connection, false to accept
		void setOnEvent(std::function<bool(NetEvent, uint32_t)> onEvent) {
			m_onEvent = onEvent;
		}
		// lambda should return true to close connection
		void setOnReceive(std::function<bool(MessageBuffer, uint32_t)> onReceive) {
			m_onReceive = onReceive;
		}

		void start(uint16_t port) {
			if (keepRunning || m_connections.size() > 0) return;
			keepRunning = true;

			asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);

			m_acceptor.open(endpoint.protocol());
			m_acceptor.bind(endpoint);
			m_acceptor.listen(1);

			waitForConnection();

			InitIOContext();
		}
		// called by you
		void disconnect(uint32_t uuid) {
			if (m_acceptor.is_open()) {
				// connection doesn't exist
				if (m_connections.find(uuid) == m_connections.end()) return;

				m_connections[uuid]->close();
			}
		}
		void stop() {
			if (!keepRunning) return;
			keepRunning = false;

			for (auto [uuid, conn] : m_connections) {
				conn->close();
			}
			if (m_acceptor.is_open())
				m_acceptor.close();
			else {
				std::cout << "server::stop - this should not happen\n";
			}

		}
		void send(MessageBuffer& msg, uint32_t uuid = -1, bool ignore = false) override {
			if (m_connections.size() == 0) {
				std::cout << "no connections\n";
				return;
			}

			if (msg.size() == 0) {
				log::out << "You tried to send no data?\n";
				return;
			}

			std::shared_ptr<MessageBuffer> ptr = std::make_shared<MessageBuffer>(msg);
			msg.noDelete = true;

			if (uuid != -1 && !ignore) {
				auto pair = m_connections.find(uuid);
				if (pair != m_connections.end()) {
					pair->second->send(ptr);
				}
			} else {
				for (auto [id, conn] : m_connections) {
					if ((uuid == id && ignore) || !ignore) {
						conn->send(ptr);
					}
				}
			}
		}

		friend class Connection;
	private:
		bool keepRunning = false;

		//std::vector<std::shared_ptr<MessageBuffer>> m_outMessages;

		asio::ip::tcp::acceptor m_acceptor;

		std::unordered_map<uint32_t, Connection*> m_connections;

		std::function<bool(NetEvent, uint32_t)> m_onEvent = nullptr;
		std::function<bool(MessageBuffer, uint32_t)> m_onReceive = nullptr;

		void clean() {
			// what should i do?
		}
		// called from within
		void _disconnect(uint32_t uuid) {

			asio::post(GetIOContext(), [this, uuid]() {

				// connection doesn't exist
				if (m_connections.find(uuid) == m_connections.end()) return;
				// double check
				auto pair = m_connections.find(uuid);
				if (pair != m_connections.end()) {

					delete pair->second;

					m_connections.erase(uuid);

					if (m_onEvent) m_onEvent(NetEvent::Disconnect, uuid);

					if (!keepRunning && m_connections.size() == 0) {
						m_onEvent(NetEvent::Stopped, -1);
					}
				}
			});
		}
		bool shutdown(std::error_code ec) {
			if (ec.value() == 995) {
				return true;
			}
			return false;
		}
		void waitForConnection() {
			if (m_acceptor.is_open()) {
				m_acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
					if (ec) {
						if (shutdown(ec)) {
							if (m_connections.size() == 0) {
								m_onEvent(NetEvent::Stopped, -1);
							} else {
								for (auto [uuid, conn] : m_connections) {
									conn->close();
								}
							}
						} else {
							stop();
							std::cout << "server error " << ec.value() << " " << ec.message() << "\n";
						}
					} else {
						uint32_t uuid = GenerateUUID();
						Connection* conn = new Connection(std::move(socket));
						m_connections[uuid] = conn;
						conn->m_uuid = uuid;
						m_connections[uuid]->m_server = this;

						waitForConnection();

						conn->readHead();

						if (m_onEvent) {
							if (m_onEvent(NetEvent::Connect, uuid)) {
								conn->close();
							}
						}
					}
				});
			}
		}
	};
	class Client : public Sender {
	public:
		Client() = default;
		~Client() {
			disconnect();
		}
		// lambda should return true to deny connection, false to accept
		void setOnEvent(std::function<bool(NetEvent)> onEvent) {
			m_onEvent = onEvent;
		}
		// lambda should return true to close connection
		void setOnReceive(std::function<bool(MessageBuffer)> onReceive) {
			m_onReceive = onReceive;
		}
		/*
		set on events before connecting
		*/
		void connect(const std::string& ip, uint16_t port) {

			if (keepRunning || m_connection) return;
			keepRunning = true;

			m_connection = new Connection(GetIOContext());
			m_connection->m_uuid = 9999;
			m_connection->m_client = this;

			asio::ip::tcp::resolver resolver(GetIOContext());
			asio::ip::tcp::resolver::results_type endpoints;
			try {
				endpoints = resolver.resolve(ip, std::to_string(port));
			} catch (std::system_error err) {
				// ip was incorrect or couldn't be found
				keepRunning = false;
				clean();
				return;
			}
			asio::async_connect(m_connection->m_socket, endpoints, [this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
				if (ec){
					clean();
					if (keepRunning) {
						m_onEvent(NetEvent::Failed);
						keepRunning = false;
					} else {
						m_onEvent(NetEvent::Disconnect);
					}
				} else {
					if (m_onEvent) {
						if (m_onEvent(NetEvent::Connect)) {
							disconnect();
							return;
						}
					}
					m_connection->readHead();
				}
			});

			InitIOContext();
		}
		// Called by you, will call close on connection.
		void disconnect() {
			if (!keepRunning) return;
			keepRunning = false;
			if (m_connection) {
				m_connection->close();
			}
		}
		// uuid and ignore is not relevant for client
		void send(MessageBuffer& msg, uint32_t uuid = -1, bool ignore = false) override {
			if (!m_connection) return;

			if (msg.size() == 0) {
				log::out << "You tried to send no data?\n";
				return;
			}

			std::shared_ptr<MessageBuffer> ptr = std::make_shared<MessageBuffer>(msg);
			m_connection->send(ptr);
			msg.noDelete = true;
		}

		friend class Connection;
	private:
		bool keepRunning = false;

		// When the connection's read and write listeners failed due to connection closing.
		// This will then be called to clean data.
		void _disconnect() {
			keepRunning = false;
			if (m_connection) {
				asio::post(GetIOContext(), [this]() {
					clean();

					if (m_onEvent) m_onEvent(NetEvent::Disconnect);

				});
			}
		}
		// Will only work when connection doesn't have any work
		void clean() {
			if (!m_connection) return;
			if (m_connection->hasWork()) return;

			// if you call close on socket it crashes sometimes. especially if it's because of async_connect or async_read

			delete m_connection;
			m_connection = nullptr;
		}

		Connection* m_connection = nullptr;

		std::function<bool(NetEvent)> m_onEvent = nullptr;
		std::function<bool(MessageBuffer)> m_onReceive = nullptr;
	};
}