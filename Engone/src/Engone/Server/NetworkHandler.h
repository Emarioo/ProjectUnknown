#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>

#define MSG_HEADER_SIZE 4

#ifndef MAX_MESSAGE_BUFFER
	#define MAX_MESSAGE_BUFFER 512
#endif

namespace engone {

	extern asio::io_context io_context;
	void InitIOContext();

	uint32_t GenerateUUID();

	class MessageBuffer {
	private:
		char* m_data = nullptr;
		uint32_t m_maxSize = 0;
		uint32_t m_readHead = 0;
		int sharings = 0;

		friend class Connection;
		friend class Client;
		friend class Server;

		bool resize(int size) {
			if (size < 4)
				size = 4;

			char* data = (char*)realloc(m_data, size);
			if (data) {
				m_data = data;
				m_maxSize = size;
				return false;
			} else {
				// failed
				return true;
			}
		}
		MessageBuffer* copy() {
			MessageBuffer* buf = new MessageBuffer(*this);
			sharings++;
			return buf;
		}
	public:
		MessageBuffer(uint32_t size = 4) {
			if (size < 4)
				size = 4;

			m_data = (char*)malloc(size);
			if (m_data) {
				m_maxSize = size;
				*((uint32_t*)m_data) = 0;
			} else {
				// failed
			}
		}
		~MessageBuffer() {
			if (m_data && sharings==0) {
				free(m_data);
			}
		}
		bool isShared() {
			return sharings > 0;
		}
		
		void unshare() {
			sharings--;
		}
		MessageBuffer* share() {
			sharings++;
			return this;
		}
		uint32_t size() {
			if (m_data) {
				return *((uint32_t*)m_data);
			}
			return 0;
		}
		template<class T>
		void operator<<(T& in) {
			if (m_data) {
				if (m_maxSize<size()+sizeof(T) ) {
					bool failed = resize((m_maxSize+sizeof(T))*2);
					if (failed) {
						// failed
						return;
					}
				}
				uint32_t head = size();
				std::memcpy(m_data+ MSG_HEADER_SIZE +head, &in, sizeof(T));
				*((uint32_t*)m_data) += sizeof(T);
			}
		}
		void operator<<(std::string& in) {
			if (m_data) {
				if (m_maxSize < size() + in.length()+1) {
					bool failed = resize((m_maxSize + in.length()+1) * 2);
					if (failed) {
						// failed
						return;
					}
				}
				uint32_t head = size();
				std::memcpy(m_data + MSG_HEADER_SIZE+ head, in.data(), in.length()+1);
				*((uint32_t*)m_data) += in.length() + 1;
			}
		}
		template<class T>
		void operator>>(T& out) {
			if (m_data) {
				if (sizeof(T) > size()) {
					// fishy
					std::cout << "Corrupted network message\n";
					return;
				}
				std::memcpy(&out, m_data + MSG_HEADER_SIZE + m_readHead, sizeof(T));
				m_readHead += sizeof(T);
				*((uint32_t*)m_data) -= sizeof(T);
			}
		}
		void operator>>(std::string& out) {
			if (m_data) {
				int length = strlen(m_data+MSG_HEADER_SIZE);
				if (length+1 > size()) {
					// fishy
					std::cout << "Corrupted network message\n";
					return;
				}
				out.append(std::string_view(m_data + MSG_HEADER_SIZE + m_readHead));
				m_readHead += length+1;// +1 for \0
				*((uint32_t*)m_data) -= length + 1;
			}
		}
		void flush() {
			*((uint32_t*)m_data) = 0;
			m_readHead = 0;
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
			if (m_socket.is_open()) {
				m_socket.close();
			}
			if (m_buffer) {
				// we share it when we create and now have to lose it
				m_buffer->unshare();
				delete m_buffer;
			}

			for (int i = 0; i < m_outMessages.size(); i++)
				m_outMessages[i]->unshare();
		}

		uint32_t m_uuid=0;
		asio::ip::tcp::socket m_socket;

		MessageBuffer* m_buffer=nullptr;
		
		Server* m_server = nullptr;
		Client* m_client = nullptr;

		void close() {
			if (m_socket.is_open()) {
				m_socket.close();
			}
		}
		bool readingHead=false, readingBody=false, writingHead=false, writingBody=false;
		bool hasWork() {
			//std::cout << m_uuid << " "<<readingHead << " " <<readingBody << " " << writingHead << " " << writingBody << "\n";
			return (readingHead + readingBody + writingHead + writingBody) != 0;
		}
		void readHead() {
			if (!m_socket.is_open()) {
				forcedShutdown(asio::error_code());
				return;
			}

			if (!m_buffer) {
				m_buffer = new MessageBuffer(MAX_MESSAGE_BUFFER);
				// prevent the data of this buffer from being deleted out of this scope
				m_buffer->share();
			}
			if (m_buffer->m_maxSize == 0) {
				// no buffer for some reason
				return;
			}

			if (readingHead)
				return;

			readingHead = true;
			asio::async_read(m_socket, asio::buffer(m_buffer->m_data, MSG_HEADER_SIZE),
				[this](std::error_code ec, std::size_t length) {
					readingHead = false;

					if (ec) {
						if (!forcedShutdown(ec))
							std::cout << "readHead error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
					} else {
						readBody();
					}
				});
		}
		void readBody();

		void send(MessageBuffer* msg) {
			if (!m_socket.is_open())
				return;

			asio::post(io_context, [this, msg]() {
				m_outMessages.push_back(msg);
				if (!writingHead && !writingBody && (readingHead||readingBody)) {
					if (m_outMessages.size() == 1) {
						writeHead();
					}
				}
			});
		}

		friend class Client;

	private:
		bool forcedShutdown(asio::error_code ec);
		void writeHead() {
			if (!m_socket.is_open())
				return;

			if (m_outMessages.size() == 0)
				return;

			if (writingHead)
				return;

			writingHead = true;
			asio::async_write(m_socket, asio::buffer(m_outMessages.front()->m_data, MSG_HEADER_SIZE),
				[this](std::error_code ec, std::size_t length) {
					writingHead = false;
					if (ec) {
						if (!forcedShutdown(ec))
							std::cout << "writeHead error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
					} else {
						writeBody();
					}
				});

		}
		void writeBody();

		std::vector<MessageBuffer*> m_outMessages;
	};

	class Server {
	public:
		Server() : m_acceptor(io_context) {}
		~Server() {
			stop();
		}

		void setOnReceive(std::function<bool(uint32_t,MessageBuffer)> onReceive) {
			m_onReceive = onReceive;
		}
		void setOnConnect(std::function<bool(uint32_t)> onConnect) {
			m_onConnect = onConnect;
		}
		void setOnDisconnect(std::function<void(uint32_t)> onDisconnect) {
			m_onDisconnect = onDisconnect;
		}

		void start(uint16_t port) {
			if (started) return;
			started = true;

			asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);

			m_acceptor.open(endpoint.protocol());
			m_acceptor.bind(endpoint);
			m_acceptor.listen(1);

			waitForConnection();

			InitIOContext();
		}
		void disconnect(uint32_t uuid) {
			if (m_acceptor.is_open()) {
				// connection doesn't exist
				if (m_connections.find(uuid) == m_connections.end()) return;

				m_connections[uuid]->close();
			}
		}
		void stop() {
			if (!started) return;

			for (auto[uuid, conn] : m_connections) {
				conn->close();
			}
			if(m_acceptor.is_open())
				m_acceptor.close();

			started = false;
		}
		void send(MessageBuffer& msg, uint32_t uuid = -1, bool ignore = false) {
			if (m_connections.size() == 0) {
				std::cout << "no connections\n";
				return;
			}

			MessageBuffer* buffer = msg.copy();
			m_outMessages.push_back(buffer);

			if (uuid != -1 && !ignore) {
				auto pair = m_connections.find(uuid);
				if (pair != m_connections.end()) {
					pair->second->send(buffer->share());
				}
			} else {
				for (auto [id, conn] : m_connections) {
					if ((uuid == id && ignore) || !ignore) {
						conn->send(buffer->share());
					}
				}
			}
		}

		friend class Connection;
		//private:
		bool started = false;

		std::vector<MessageBuffer*> m_outMessages;

		asio::ip::tcp::acceptor m_acceptor;

		std::unordered_map<uint32_t, Connection*> m_connections;

		std::function<bool(uint32_t,MessageBuffer)> m_onReceive = nullptr;
		std::function<bool(uint32_t)> m_onConnect = nullptr;
		std::function<void(uint32_t)> m_onDisconnect = nullptr;

		void _disconnect(uint32_t uuid) {
			asio::post(io_context, [this, uuid]() {

				// connection doesn't exist
				if (m_connections.find(uuid) == m_connections.end()) return;
				// double check
				auto pair = m_connections.find(uuid);
				if (pair != m_connections.end()) {

					delete pair->second;

					// delete unshared messages
					for (int i = 0; i < m_outMessages.size(); i++) {
						MessageBuffer* buf = m_outMessages[i];
						if (!buf->isShared()) {
							delete buf;
							m_outMessages.erase(m_outMessages.begin() + i);
							i--;
						}
					}

					m_connections.erase(uuid);

					if (m_onDisconnect)
						m_onDisconnect(uuid);
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
							std::cout << "stopped server\n";
						} else {
							std::cout << "server error " << ec.value() << " " << ec.message() << "\n";
						}
					} else {
						std::cout << "accepted connection " << socket.remote_endpoint() << "\n";

						uint32_t uuid = GenerateUUID();
						Connection* conn = new Connection(std::move(socket));
						m_connections[uuid] = conn;
						conn->m_uuid = uuid;
						m_connections[uuid]->m_server = this;

						waitForConnection();

						conn->readHead();

						if (m_onConnect) {
							if (!m_onConnect(uuid)) {
								conn->close();
								//delete conn;
								//m_connections.erase(uuid);
								//return;
							}
						}

					}
					});
			}
		}
	};
	class Client {
	public:
		Client() = default;
		~Client() {
			disconnect();
		}

		void setOnReceive(std::function<bool(MessageBuffer)> onReceive) {
			m_onReceive = onReceive;
		}
		void setOnConnect(std::function<bool()> onConnect) {
			m_onConnect = onConnect;
		}
		void setOnDisconnect(std::function<void()> onDisconnect) {
			m_onDisconnect = onDisconnect;
		}
		/*
		set on events before connecting
		*/
		void connect(const std::string& ip, uint16_t port) {
			if (m_connection) return;

			m_connection = new Connection(io_context);
			m_connection->m_uuid = 9999;
			m_connection->m_client = this;

			asio::ip::tcp::resolver resolver(io_context);
			asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(ip, std::to_string(port));
			
			asio::async_connect(m_connection->m_socket, endpoints, [this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
				if (ec) {
					std::cout << "client not connected\n";
				} else {
					std::cout << "client connected "<< endpoint <<" \n";

					if (m_onConnect) {
						if (!m_onConnect()) {
							disconnect();
							return;
						}
					}
					m_connection->readHead();
					m_connection->writeHead();
				}
			});

			InitIOContext();
		}
		void disconnect() {
			if (m_connection) {
				m_connection->close();
			}
		}
		void send(MessageBuffer& msg) {
			if (!m_connection) return;
		
			MessageBuffer* buffer = msg.copy();
			m_outMessages.push_back(buffer);
			m_connection->send(buffer->share());
		}
		
	//private:

		void _disconnect() {
			if (m_connection) {
				asio::post(io_context, [this]() {

					delete m_connection;
					m_connection = nullptr;

					for (int i = 0; i < m_outMessages.size(); i++)
						delete m_outMessages[i];
					m_outMessages.clear();

					if (m_onDisconnect)
						m_onDisconnect();

					});
			}
		}
		
		Connection* m_connection=nullptr;

		std::vector<MessageBuffer*> m_outMessages;

		std::function<bool(MessageBuffer)> m_onReceive=nullptr;
		std::function<bool()> m_onConnect=nullptr;
		std::function<void()> m_onDisconnect=nullptr;

	};
}