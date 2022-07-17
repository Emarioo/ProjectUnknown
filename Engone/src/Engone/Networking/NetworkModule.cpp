#include "Engone/NetworkModule.h"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>

#ifndef MAX_MESSAGE_BUFFER
#define MAX_MESSAGE_BUFFER 100
#endif

namespace engone {

	/*
		OTHER
	*/
	static asio::io_context io_context;
	asio::io_context& GetIOContext() {
		return io_context;
	}
	uint32_t lastUUID = 0;
	uint32_t GenerateUUID() {
		return ++lastUUID;
	}
	static std::thread* thrContext;
	static bool running_context = false;
	void InitIOContext() {
		if (running_context) {
			return;
		}
		running_context = true;

		if (thrContext) {
			if (thrContext->joinable()) {
				thrContext->join();
				delete thrContext;
			}
		}
		thrContext = new std::thread([&]() {
			io_context.run();
			io_context.restart();
			running_context = false;
			//std::cout << "stopped context" << "\n";
		});
	}
	void DestroyIOContext() {
		if(thrContext)
			thrContext->join();
	}
	/*
		NETEVENT
	*/
	std::string toString(NetEvent t) {
		switch (t) {
		case NetEvent::Connect: return "Connect";
		case NetEvent::Disconnect: return "Disconnect";
		case NetEvent::Failed: return "Failed";
		case NetEvent::Stopped: return "Stopped";
		}
		return "";
	}
	log::logger operator<<(log::logger a, NetEvent b) {
		return a << toString(b);
	}
	/*
		MESSAGEBUFFER
	*/
	MessageBuffer::MessageBuffer(int header, int size) {
		resize(size);
		push(header);
	}
	MessageBuffer::~MessageBuffer() {
		if (m_data && !noDelete) {
			free(m_data);
		}
	}
	bool MessageBuffer::resize(uint32_t size) {
		if (size <= m_maxSize)
			size = 2u * m_maxSize;

		char* data;
		if (!m_data) {
			data = (char*)malloc(size);
			if (data)
				*((uint32_t*)data) = 4; // 4 bytes for storing size of buffer
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
	uint32_t MessageBuffer::size() {
		if (m_data) {
			return *((uint32_t*)m_data);
		}
		return 0;
	}
	char* MessageBuffer::pushBuffer(uint32_t count) {
		push(&count);
		if (count == 0)
			return nullptr;
		if (m_data) {
			uint32_t head = size();
			if (m_maxSize < head + count) {
				if (!resize((m_maxSize + count) * 2)) {
					// failed
					return nullptr;
				}
			}
			*((uint32_t*)m_data) += count;
			return m_data + head;
		}
		return nullptr;
	}
	void MessageBuffer::push(const std::string& in) {
		if (m_data) {
			uint32_t head = size();
			if (m_maxSize < head + in.length() + 1) {
				if (!resize((m_maxSize + in.length() + 1) * 2)) {
					// failed
					return;
				}
			}
			std::memcpy(m_data + head, in.data(), in.length() + 1);
			*((uint32_t*)m_data) += in.length() + 1;
		}
	}
	void MessageBuffer::push(const char* in) {
		if (m_data) {
			uint32_t len = std::strlen(in) + 1;
			uint32_t head = size();
			if (m_maxSize < head + len) {
				if (!resize((m_maxSize + len) * 2)) {
					// failed
					return;
				}
			}
			std::memcpy(m_data + head, in, len);
			*((uint32_t*)m_data) += len;
		}
	}
	char* MessageBuffer::pullBuffer(uint32_t* count) {
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
	void MessageBuffer::pull(std::string& out) {
		if (m_data) {
			uint32_t length = strlen(m_data + m_readHead) + 1;
			if (length > size() - m_readHead) {
				// fishy
				std::cout << "Corrupted network message\n";
				return;
			}
			out.append(std::string_view(m_data + m_readHead));
			m_readHead += length;
		}
	}
	void MessageBuffer::flush() {
		*((uint32_t*)m_data) = HEAD_SIZE; // first for bytes for the size of the buffer
		m_readHead = HEAD_SIZE;
	}
	/*
		CONNECTION
	*/
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
				}
				catch (asio::system_error se) {
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
	bool Connection::forcedShutdown(asio::error_code ec) {
		if (ec.value() == 0 || // manual
			ec.value() == 10054 || //
			ec.value() == 10009 || //
			ec.value() == 995 || // program requested stop
			ec.value() == 10053 || // established connection broken - disconnect was called in onConnect func? 
			ec.value() == 1236 || // 
			ec.value() == 995 || // socket.cancel
			ec.value() == 2) // end of file
		{
			if (!hasWork()) {
				if (m_server) {
					m_server->_disconnect(m_uuid);
				}
				if (m_client) {
					m_client->_disconnect();
				}
			}

			return true;
		}
		return false;
	}
	void Connection::readHead() {
		if (!m_socket.is_open()) {
			forcedShutdown(asio::error_code());
			return;
		}

		if (!m_buffer) {
			m_buffer = new MessageBuffer();
			m_buffer->resize(MAX_MESSAGE_BUFFER);
			// prevent the data of this buffer from being deleted out of this scope
		}
		if (m_buffer->m_maxSize == 0) {
			// no buffer for some reason
			return;
		}

		if (readingHead)
			return;

		readingHead = true;
		asio::async_read(m_socket, asio::buffer(m_buffer->m_data, MessageBuffer::HEAD_SIZE),
			[this](std::error_code ec, std::uint32_t length) {
			readingHead = false;
			if (ec) {
				if (!forcedShutdown(ec))
					std::cout << "readHead error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
			} else {
				// resize if necessary
				if (m_buffer->m_maxSize < m_buffer->size()) {
					m_buffer->resize((int)(m_buffer->size() * 1.5f));
				}
				readBody();
			}
		});
	}
	void Connection::readBody() {
		if (!m_socket.is_open())
			return;
		readingBody = true;
		asio::async_read(m_socket, asio::buffer(m_buffer->m_data + MessageBuffer::HEAD_SIZE, m_buffer->size() - MessageBuffer::HEAD_SIZE),
			[this](std::error_code ec, std::uint32_t length) {
			readingBody = false;
			if (ec) {
				if (!forcedShutdown(ec))
					std::cout << "readBody error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
			} else {
				MessageBuffer copy = *m_buffer;

				copy.noDelete = true;
				if (m_server) {
					if (m_server->m_onReceive) {
						bool stay = m_server->m_onReceive(copy, m_uuid);
						m_buffer->flush();
						if(!stay)
							m_server->disconnect(m_uuid);
					}
				}
				if (m_client) {
					if (m_client->m_onReceive) {
						bool stay =  m_client->m_onReceive(copy);
						m_buffer->flush();
						if (!stay)
							m_client->disconnect();
					}
				}
				readHead();
			}
		});
	}
	void Connection::writeHead() {
		if (!m_socket.is_open()) return;
		if (m_outMessages.size() == 0) return;
		if (writingHead) return;

		writingHead = true;
		asio::async_write(m_socket, asio::buffer(m_outMessages.front()->m_data, MessageBuffer::HEAD_SIZE),
			[this](std::error_code ec, std::uint32_t length) {
			writingHead = false;
			if (ec) {
				if (!forcedShutdown(ec))
					std::cout << "writeHead error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
			} else {
				writeBody();
			}
		});
	}
	void Connection::writeBody() {
		if (!m_socket.is_open())
			return;
		writingBody = true;

		asio::async_write(m_socket, asio::buffer(m_outMessages.front()->m_data + MessageBuffer::HEAD_SIZE, m_outMessages.front()->size() - MessageBuffer::HEAD_SIZE),
			[this](std::error_code ec, std::uint32_t length) {
			writingBody = false;
			if (ec) {
				if (!forcedShutdown(ec))
					std::cout << "writeBody error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
			} else {

				m_outMessages.erase(m_outMessages.begin());

				if (m_outMessages.size() > 0) {
					writeHead();
				}
			}
		});
	}
	/*
		SERVER
	*/
	Server::Server()
		//: test(GetIOContext()) {}
		: m_acceptor(new asio::ip::tcp::acceptor(GetIOContext())) {}
	Server::~Server() {
		stop();
		//std::cout << "del\n";
		//asio::ip::tcp::acceptor def(GetIOContext());
		//((asio::ip::tcp::acceptor*)m_acceptor)->release();
		//((asio::ip::tcp::acceptor*)m_acceptor)->close();
		//((asio::ip::tcp::acceptor*)m_acceptor)->release();
		delete ((asio::ip::tcp::acceptor*)m_acceptor);
		//delete m_acceptor;
	}
	void Server::setOnEvent(std::function<bool(NetEvent, uint32_t)> onEvent) {
		m_onEvent = onEvent;
	}
	void Server::setOnReceive(std::function<bool(MessageBuffer, uint32_t)> onReceive) {
		m_onReceive = onReceive;
	}
	void Server::start(const std::string& port) {
		if (keepRunning || m_connections.size() > 0) return;
		keepRunning = true;

		uint16_t intPort = 0; 
		try {
			intPort = std::stoi(port);
		} catch (std::invalid_argument err) {
			return;
		}

		asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), intPort);

		((asio::ip::tcp::acceptor*)m_acceptor)->open(endpoint.protocol());
		((asio::ip::tcp::acceptor*)m_acceptor)->bind(endpoint);
		((asio::ip::tcp::acceptor*)m_acceptor)->listen(1);

		waitForConnection();

		InitIOContext();
	}
	void Server::disconnect(uint32_t uuid) {
		if (((asio::ip::tcp::acceptor*)m_acceptor)->is_open()) {
			// connection doesn't exist
			if (m_connections.find(uuid) == m_connections.end()) return;

			m_connections[uuid]->close();
		}
	}
	void Server::stop() {
		if (!keepRunning) return;
		keepRunning = false;

		for (auto [uuid, conn] : m_connections) {
			conn->close();
		}
		if (((asio::ip::tcp::acceptor*)m_acceptor)->is_open())
			((asio::ip::tcp::acceptor*)m_acceptor)->close();
		else {
			std::cout << "server::stop - this should not happen\n";
		}
	}
	void Server::_disconnect(uint32_t uuid) {

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
	bool Server::shutdown(std::error_code ec) {
		if (ec.value() == 995) {
			return true;
		}
		return false;
	}
	void Server::waitForConnection() {
		if (((asio::ip::tcp::acceptor*)m_acceptor)->is_open()) {
			((asio::ip::tcp::acceptor*)m_acceptor)->async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
				if (ec) {
					if (shutdown(ec)) {
						if (m_connections.size() == 0) {
							m_onEvent(NetEvent::Stopped, -1);
						}
						else {
							for (auto [uuid, conn] : m_connections) {
								conn->close();
							}
						}
					}
					else {
						stop();
						std::cout << "server error " << ec.value() << " " << ec.message() << "\n";
					}
				}
				else {
					uint32_t uuid = GenerateUUID();
					Connection* conn = new Connection(std::move(socket));
					m_connections[uuid] = conn;
					conn->m_uuid = uuid;
					m_connections[uuid]->m_server = this;

					waitForConnection();

					conn->readHead();

					if (m_onEvent) {
						if (!m_onEvent(NetEvent::Connect, uuid)) {
							conn->close();
						}
					}
				}
				});
		}
	}
	void Server::send(MessageBuffer& msg, uint32_t uuid, bool ignore) {
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
		}
		else {
			for (auto [id, conn] : m_connections) {
				if ((uuid == id && ignore) || !ignore) {
					conn->send(ptr);
				}
			}
		}
	}
	/*
		CLIENT
	*/
	Client::~Client() {
		disconnect();
	}
	void Client::setOnEvent(std::function<bool(NetEvent)> onEvent) {
		m_onEvent = onEvent;
	}
	void Client::setOnReceive(std::function<bool(MessageBuffer)> onReceive) {
		m_onReceive = onReceive;
	}
	void Client::connect(const std::string& ip, const std::string& port) {
		if (keepRunning || m_connection) return;
		keepRunning = true;

		m_connection = new Connection(GetIOContext());
		m_connection->m_uuid = 9999;
		m_connection->m_client = this;

		asio::ip::tcp::resolver resolver(GetIOContext());
		asio::ip::tcp::resolver::results_type endpoints;
		try {
			endpoints = resolver.resolve(ip, port);
		}
		catch (std::system_error err) {
			// ip was incorrect or couldn't be found
			keepRunning = false;
			clean();
			return;
		}
		asio::async_connect(m_connection->m_socket, endpoints, [this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
			if (ec) {
				clean();
				if (keepRunning) {
					m_onEvent(NetEvent::Failed);
					keepRunning = false;
				}
				else {
					m_onEvent(NetEvent::Disconnect);
				}
			}
			else {
				if (m_onEvent) {
					if (!m_onEvent(NetEvent::Connect)) {
						disconnect();
						return;
					}
				}
				m_connection->readHead();
			}
			});

		InitIOContext();
	}
	void Client::disconnect() {
		if (!keepRunning) return;
		keepRunning = false;
		if (m_connection) {
			m_connection->close();
		}
	}
	void Client::send(MessageBuffer& msg, uint32_t uuid, bool ignore) {
		if (!m_connection) return;

		if (msg.size() == 0) {
			log::out << "You tried to send no data?\n";
			return;
		}

		std::shared_ptr<MessageBuffer> ptr = std::make_shared<MessageBuffer>(msg);
		m_connection->send(ptr);
		msg.noDelete = true;
	}
	void Client::clean() {
		if (!m_connection) return;
		if (m_connection->hasWork()) return;

		// if you call close on socket it crashes sometimes. especially if it's because of async_connect or async_read

		delete m_connection;
		m_connection = nullptr;
	}
	void Client::_disconnect() {
		keepRunning = false;
		if (m_connection) {
			asio::post(GetIOContext(), [this]() {
				clean();

				if (m_onEvent) m_onEvent(NetEvent::Disconnect);

				});
		}
	}
}