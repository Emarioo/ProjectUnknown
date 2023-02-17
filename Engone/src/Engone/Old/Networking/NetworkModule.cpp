#include "Engone/NetworkModule.h"
#include "Engone/Utilities/Utilities.h"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>

#include "Engone/Utilities/Tracker.h"

namespace engone {
	TrackerId MessageBuffer::trackerId="MessageBuffer";
	TrackerId Server::trackerId="Server";
	TrackerId Client::trackerId="Client";
	static TrackerId ioContextTrackerId = "asio::io_context";
	static TrackerId acceptorTrackerId = "asio::acceptor";
	static TrackerId threadTrackerId = "std::thread";
	/*
		OTHER
	*/
	//static asio::io_context io_context;
	static asio::io_context* io_context=nullptr;
	asio::io_context& GetIOContext() {
		if (!io_context) {
			io_context = new asio::io_context();
			GetTracker().track({ ioContextTrackerId,sizeof(asio::io_context),io_context }); // io_context doesn't have trackerId
		}
		return *io_context;
	}
	static std::thread* thrContext;
	static bool running_context = false;
	bool HasIOContext() {
		return io_context!=nullptr;
	}
	void InitIOContext() {
		if (running_context) {
			return;
		}
		running_context = true;
		if (thrContext) {
			if (thrContext->joinable()) {
				thrContext->join();
				GetTracker().untrack({ threadTrackerId,sizeof(std::thread),thrContext });
				delete thrContext;
				thrContext = nullptr;
			}
		}
		thrContext = new std::thread([&]() {
			GetIOContext().restart();
			GetIOContext().run();
			running_context = false;
			//std::cout << "stopped context" << "\n";
		});
		GetTracker().track({ threadTrackerId,sizeof(std::thread),thrContext });
	}
	void DestroyIOContext() {
		if (!io_context) return;
		GetIOContext().stop();
		if (thrContext) {
			if (thrContext->joinable()) {
				thrContext->join();
				GetTracker().untrack({ threadTrackerId,sizeof(std::thread),thrContext });
				delete thrContext;
				thrContext = nullptr;
			}
		}
		bool hasStopped = io_context->stopped();
		// will crash, something with decref, not sure why. just asio being asio i guess. (no honestly, i probably forgot to close some socket)
		if (io_context != nullptr) {
			GetTracker().untrack({ ioContextTrackerId,sizeof(asio::io_context),io_context });
			delete io_context;
			io_context = nullptr;
		}
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
	Logger& operator<<(Logger& log, NetEvent value) {
		return log << toString(value);
	}
	/*
		MESSAGEBUFFER
	*/
	//MessageBuffer* MessageBuffer::Create(int msgType, uint32_t size = 0) {
	//	MessageBuffer* out = (MessageBuffer*)tracker::_malloc(sizeof(MessageBuffer));
	//	memset(out, 0, sizeof(MessageBuffer));
	//	if (size != 0)
	//		out->resize(size);
	//	out->push(msgType);
	//	return out;
	//}
	MessageBuffer::MessageBuffer(int msgType, uint32_t size) {
		if(size!=0)
			resize(size);
		push(msgType);
	}
	MessageBuffer::~MessageBuffer() {
		if (m_data && !noDelete) {
			Free(m_data, m_dataSize);
			GetTracker().subMemory(trackerId,m_dataSize);
		}
	}
	bool MessageBuffer::resize(uint32_t size) {
		char* data;
		if (!m_data) {
			data = (char*)Allocate(size+sizeof(uint32_t));
			GetTracker().addMemory(trackerId, size + sizeof(uint32_t));
			if (data)
				*((uint32_t*)data) = 0;
		} else {
			data = (char*)Reallocate(m_data, m_dataSize, size + sizeof(uint32_t));
			GetTracker().subMemory(trackerId, m_dataSize);
			GetTracker().addMemory(trackerId, size + sizeof(uint32_t));
		}
		if (data) {
			m_data = data;
			// Hello, i move addMem, subMem here and i'm not sure if I did it correctly an i'm to lazy to test it. Thanks future me.

			//GetTracker().subMemory(trackerId,m_dataSize);
			//GetTracker().addMemory(trackerId,size + sizeof(uint32_t));
			m_dataSize = size+sizeof(uint32_t);
			return true;
		} else {
			log::out << log::RED << "MessageBuffer::resize - malloc or realloc failed\n";
			return false;
		}
	}
	char* MessageBuffer::pushBuffer(uint32_t bytes) {
		if (bytes == 0) {
			log::out << log::RED << "MessageBuffer::pushBuffer - bytes cannot be zero\n";
			return nullptr;
		}
		push(&bytes);
		if (m_dataSize < sizeof(uint32_t)+size() + bytes) {
			if (!resize((m_dataSize + bytes) * 2)) {
				// resize will give error
				return nullptr;
			}
		}
		uint32_t head = sizeof(uint32_t)+size();
		*((uint32_t*)m_data) += bytes;
		return m_data + head;
	}
	void MessageBuffer::push(const std::string& in) {
		if (m_dataSize < sizeof(uint32_t) + size() + in.length() + 1) {
			if (!resize((m_dataSize + in.length() + 1) * 2)) {
				// resize will give error
				return;
			}
		}
		uint32_t head = sizeof(uint32_t)+size();
		memcpy_s(m_data + head, m_dataSize -head, in.data(), in.length() + 1);
		*((uint32_t*)m_data) += in.length() + 1;
	}
	void MessageBuffer::push(const char* in) {
		uint32_t len = strlen(in) + 1;
		if (m_dataSize  < sizeof(uint32_t) + size() + len) {
			if (!resize((m_dataSize + len) * 2)) {
				// resize will give error
				return;
			}
		}
		uint32_t head = sizeof(uint32_t)+size();
		memcpy_s(m_data + head, m_dataSize -head, in, len);
		*((uint32_t*)m_data) += len;
	}
	char* MessageBuffer::pullBuffer(uint32_t* bytes) {
		if (*bytes == 0)
			return nullptr;
		pull(bytes);
		if (*bytes > size() - m_readHead) {
			// fishy
			log::out <<log::RED<< "MessageBuffer::pullBuffer - bytes of pulled buffer exceeds message!\n";
			return nullptr;
		}
		char* out = m_data+sizeof(uint32_t)+m_readHead;
		m_readHead += *bytes;
		return out;
	}
	void MessageBuffer::pull(std::string& out) {
		if (m_data) {
			uint32_t length = strnlen_s(m_data + sizeof(uint32_t)+m_readHead,size()-m_readHead) + 1;
			if (length > size() - m_readHead) {
				// fishy
				log::out << log::RED << "MessageBuffer::pull - length of string exceeds message!\n";
				return;
			}
			out.append(std::string_view(m_data +sizeof(uint32_t)+ m_readHead));
			m_readHead += length;
		}
	}
	void MessageBuffer::flush() {
		if (m_data) {
			*((uint32_t*)m_data) = 0;
			m_readHead = 0;
		}
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
				GetTracker().untrack(m_buffer);
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
				if (!writingMessage && (readingHead || readingBody||flushingBody)) {
					if (m_outMessages.size() >= 1) {
						writeMessage();
					}
				}
				});
		}
		UUID m_uuid = 0;

		static TrackerId trackerId;
	private:
		friend class Client;
		friend class Server;

		asio::ip::tcp::socket m_socket;

		MessageBuffer* m_buffer = nullptr;

		Server* m_server = nullptr;
		Client* m_client = nullptr;

		bool readingHead = false, readingBody = false, flushingBody=false, writingMessage = false;
		bool hasWork() {
			//std::cout << m_uuid << " "<<readingHead << " " <<readingBody << " " << writingHead << " " << writingBody << "\n";
			return (readingHead + readingBody + flushingBody + writingMessage) != 0;
		}
		bool forcedShutdown(asio::error_code ec);
		void readHead();
		void readBody();
		uint32_t flushingLeft = 0;
		void flushBody();
		// will write and send the front message from m_outMessages.
		// will do nothing if m_outMessages is empty.
		void writeMessage();

		std::vector<std::shared_ptr<MessageBuffer>> m_outMessages;
	};
	TrackerId Connection::trackerId = "Connection";
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
			// Normal quit
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
		if (readingHead || readingBody || flushingBody) return;
		if (!m_socket.is_open()) {
			forcedShutdown(asio::error_code());
			return;
		}

		if (!m_buffer) {
			m_buffer = new MessageBuffer();
			GetTracker().track(m_buffer);
			m_buffer->resize(100);
			// prevent the data of this buffer from being deleted out of this scope
		}
		if (!m_buffer->m_data) {
			// no buffer for some reason
			log::out << log::RED << "Connection::readHead - Buffer failed!\n";
			return;
		}


		readingHead = true;
		// if the client send head and then a smaller size then expected. readBody will freeze until that data has come through.
		// This will only happend if the a programmer tampers with the client code. The same is true for server.
		asio::async_read(m_socket, asio::buffer(m_buffer->m_data, sizeof(uint32_t)),
			[this](std::error_code ec, uint32_t length) {
			readingHead = false;
			if (ec) {
				if (!forcedShutdown(ec)) {
					log::out << "readHead error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
					log::out << "Reader broken\n";
				}
			} else {
				// resize if necessary
				// a hardcoded limit, temporary solution for flaw of being able to allocate 4GB of memory.
				// not sure what happens with the rest of the data.
				uint32_t size = m_buffer->size();
				//log::out << "readHead " << size << "\n";
				uint32_t limit = 0;
				if (m_server) limit = m_server->getTransferLimit();
				if (m_client) limit = m_client->getTransferLimit();
				if (size <= limit) {
					if (m_buffer->m_dataSize-sizeof(uint32_t) < size) {
						m_buffer->resize(size);
					}
					readBody();
				} else {
					if (m_buffer->m_dataSize - sizeof(uint32_t) < limit) {
						m_buffer->resize(limit);
					}
					// FEATURE: warning when message buffer is resized to something big, suggest streaming data in that case? same with 
					// FEATURE: data streaming? with files? uuid for every message, then part 1,2,3... for the split up parts of the whole data?
					// increase read limit or implement data streaming.
					log::out << log::RED << "Connectiong::readHead - Read limit "<< limit <<" reached! skipping "<<size << " bytes.\n";
					flushingLeft = size;
					flushBody();
				}
			}
		});
	}
	void Connection::readBody() {
		if (readingHead || readingBody || flushingBody) return;
		if (!m_socket.is_open()) return;

		readingBody = true;
		asio::async_read(m_socket, asio::buffer(m_buffer->m_data + sizeof(uint32_t), m_buffer->size()),
			[this](std::error_code ec, uint32_t length) {
			readingBody = false;
			if (ec) {
				if (!forcedShutdown(ec)) {
					log::out << log::RED << "readBody error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
					log::out << "Reader is broken\n";
				}
			} else {
				MessageBuffer copy = *m_buffer;
				copy.noDelete = true;
				if (m_server) {
					m_server->getStats().m_receivedMessages++;
					m_server->getStats().m_receivedBytes += length;
					if (m_server->m_onReceive) {
						bool stay = m_server->m_onReceive(copy, m_uuid);
						m_buffer->flush();
						if(!stay)
							m_server->disconnect(m_uuid);
					}
				}
				if (m_client) {
					m_client->getStats().m_receivedMessages++;
					m_client->getStats().m_receivedBytes += length;
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
	void Connection::flushBody() {
		if (readingHead||readingBody||flushingBody) return;
		if (!m_socket.is_open()) return;
		//log::out << "FLUSHING...\n";
		uint32_t size = flushingLeft;
		if (size > m_buffer->m_dataSize - sizeof(uint32_t))
			size = m_buffer->m_dataSize - sizeof(uint32_t);
		
		flushingBody = true;
		asio::async_read(m_socket, asio::buffer(m_buffer->m_data, size),
			[this](std::error_code ec, uint32_t length) {
			flushingBody = false;

			flushingLeft -= length;
			//log::out << "Flushed " << length << ". " << flushingLeft << " left\n";
			if (ec) {
				if (!forcedShutdown(ec)) {
					log::out << log::RED << "flushBody error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
					log::out << "Reader is broken\n";
				}
			} else {
				if (flushingLeft == 0)
					readHead();
				else
					flushBody();
			}
		});
	}
	void Connection::writeMessage() {
		if (writingMessage) return;
		if (!m_socket.is_open()) return;
		if (m_outMessages.size() == 0) return;

		writingMessage = true;
		asio::async_write(m_socket, asio::buffer(m_outMessages.front()->m_data, m_outMessages.front()->size() + sizeof(uint32_t)),
			[this](std::error_code ec, uint32_t length) {
			writingMessage = false;
			if (ec) {
				if (!forcedShutdown(ec))
					log::out << "writeBody error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
			} else {
				if (m_server) {
					m_server->getStats().m_sentMessages++;
					m_server->getStats().m_sentBytes += length;
				} else if (m_client) {
					m_client->getStats().m_sentMessages++;
					m_client->getStats().m_sentBytes += length;
				}
				m_outMessages.erase(m_outMessages.begin());

				if (m_outMessages.size() > 0) {
					writeMessage();
				}
			}
		});
	}
	/*
		SERVER
	*/
	Server::Server()
		//: test(GetIOContext()) {}
		: Sender(true)
	{
		m_acceptor = new asio::ip::tcp::acceptor(GetIOContext());
		GetTracker().track({ acceptorTrackerId,sizeof(asio::ip::tcp::acceptor),m_acceptor });
	}
	Server::~Server() {
		stop();
		
		delete m_acceptor;
		GetTracker().untrack({ acceptorTrackerId,sizeof(asio::ip::tcp::acceptor),m_acceptor });
	}
	void Server::setOnEvent(std::function<bool(NetEvent, UUID)> onEvent) {
		m_onEvent = onEvent;
	}
	void Server::setOnReceive(std::function<bool(MessageBuffer, UUID)> onReceive) {
		m_onReceive = onReceive;
	}
	bool Server::start(const std::string& port) {
		if (keepRunning || m_connections.size() > 0) return true;
		keepRunning = true;

		uint16_t intPort = 0; 
		try {
			intPort = std::stoi(port);
		} catch (std::invalid_argument err) {
			return false;
		}

		asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), intPort);

		((asio::ip::tcp::acceptor*)m_acceptor)->open(endpoint.protocol());
		((asio::ip::tcp::acceptor*)m_acceptor)->bind(endpoint);
		((asio::ip::tcp::acceptor*)m_acceptor)->listen(1);

		waitForConnection();

		InitIOContext();
		return true;
	}
	void Server::disconnect(UUID uuid) {
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
			log::out <<log::RED<< "Server::stop - this should not happen\n";
		}
	}
	void Server::_disconnect(UUID uuid) {
		asio::post(GetIOContext(), [this, uuid]() {
			// connection doesn't exist
			if (m_connections.find(uuid) == m_connections.end()) return;
			// double check
			auto pair = m_connections.find(uuid);
			if (pair != m_connections.end()) {

				GetTracker().untrack(pair->second);
				delete pair->second;

				m_connections.erase(uuid);

				if (m_onEvent) m_onEvent(NetEvent::Disconnect, uuid);

				if (!keepRunning && m_connections.size() == 0) {
					m_onEvent(NetEvent::Stopped, 0);
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
							m_onEvent(NetEvent::Stopped, 0);
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
					UUID uuid = UUID::New();
					Connection* conn = new Connection(std::move(socket));
					GetTracker().track(conn);

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
	void Server::send(MessageBuffer& msg, UUID uuid, bool ignore) {
		if (m_connections.size() == 0) {
			//log::out << "no connections\n";
			return;
		}
		if (msg.size() == 0) {
			log::out <<log::YELLOW<< "You tried to send no data?\n";
			return;
		}
		if (msg.size() > getTransferLimit()) {
			log::out << log::RED << "Buffer is bigger than the transfer limit!\n";
			return;
		}

		// ISSUE: how to track memory of shared ptr.
		std::shared_ptr<MessageBuffer> ptr = std::make_shared<MessageBuffer>(msg);
		msg.noDelete = true;

		if (uuid != 0 && !ignore) {
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
	Client::Client() : Sender(false) {}
	Client::~Client() {
		disconnect();
	}
	void Client::setOnEvent(std::function<bool(NetEvent)> onEvent) {
		m_onEvent = onEvent;
	}
	void Client::setOnReceive(std::function<bool(MessageBuffer)> onReceive) {
		m_onReceive = onReceive;
	}
	bool Client::connect(const std::string& ip, const std::string& port) {
		if (keepRunning || m_connection) return true;
		keepRunning = true;

		m_connection = new Connection(GetIOContext());
		GetTracker().track(m_connection);
		
		m_connection->m_uuid = 0; // ISSUE: this was 9999 before i switched to UUID maybe bad?
		m_connection->m_client = this;

		asio::ip::tcp::resolver resolver(GetIOContext());
		asio::ip::tcp::resolver::results_type endpoints;
		try {
			endpoints = resolver.resolve(ip, port);
		} catch (std::system_error err) {
			// ip was incorrect or couldn't be found
			keepRunning = false;
			clean();
			return false;
		}
		asio::async_connect(m_connection->m_socket, endpoints, [this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
			if (ec) {
				clean();
				if (keepRunning) {
					m_onEvent(NetEvent::Failed);
					keepRunning = false;
				} else {
					m_onEvent(NetEvent::Disconnect);
				}
			} else {
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
		return true;
	}
	void Client::disconnect() {
		if (!keepRunning) return;
		keepRunning = false;
		if (m_connection) {
			m_connection->close();
		}
	}
	void Client::send(MessageBuffer& msg, UUID uuid, bool ignore) {
		if (!m_connection) return;

		if (msg.size() == 0) {
			log::out << log::RED << "You tried to send no data?\n";
			return;
		}
		if (msg.size() > getTransferLimit()) {
			log::out << log::RED << "Buffer is bigger than the transfer limit!\n";
			return;
		}
		// ISSUE: how to track memory of shared ptr. increment sizeof(MessageBuffer) when is it destroyed?
		// Make a custom ptr? heeeeeellllllp...
		std::shared_ptr<MessageBuffer> ptr = std::make_shared<MessageBuffer>(msg);
		m_connection->send(ptr);
		msg.noDelete = true;
	}
	void Client::clean() {
		if (!m_connection) return;
		if (m_connection->hasWork()) return;

		// if you call close on socket it crashes sometimes. especially if it's because of async_connect or async_read
		// hasn't crashed in a while, might have fixed it? - 2022-07-19

		GetTracker().untrack(m_connection);
		delete m_connection;
		m_connection = nullptr;
	}
	void Client::_disconnect() {
		keepRunning = false;
		if (m_connection) {
			asio::post(GetIOContext(), [this]() {
				clean();

				if (m_onEvent)
					m_onEvent(NetEvent::Disconnect);

			});
		}
	}
}