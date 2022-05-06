#include "NetworkHandler.h"

namespace engone {

	static asio::io_context io_context;
	asio::io_context& GetIOContext(){
		return io_context;
	}
	//static int messageBuffers = 0;
	MessageBuffer::MessageBuffer(int header, int size) {
		resize(size);
		push(header);
		//messageBuffers++;
		//std::cout << "buffer++ : " << messageBuffers << "\n";
	}
	MessageBuffer::MessageBuffer() {
		//messageBuffers++;
		//std::cout << "buffer++ : " << messageBuffers << "\n";
	}
	MessageBuffer::~MessageBuffer() {
		//messageBuffers--;
		//std::cout << "buffers-- : " << messageBuffers << "\n";
		if (m_data && !noDelete) {
			free(m_data);
		}
	}
	static std::thread* thrContext;
	static bool running_context=false;
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
	uint32_t lastUUID = 0;
	uint32_t GenerateUUID() {
		return ++lastUUID;
	}
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
			[this](std::error_code ec, std::size_t length) {
			readingHead = false;
			if (ec) {
				if (!forcedShutdown(ec))
					std::cout << "readHead error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
			} else {
				// resize if necessary
				if (m_buffer->m_maxSize < m_buffer->size()) {
					m_buffer->resize(m_buffer->size()*1.5);
				}
				readBody();
			}
		});
	}
	void Connection::readBody() {
		if (!m_socket.is_open())
			return;
		readingBody = true;
		asio::async_read(m_socket, asio::buffer(m_buffer->m_data + MessageBuffer::HEAD_SIZE, m_buffer->size()-MessageBuffer::HEAD_SIZE),
			[this](std::error_code ec, std::size_t length) {
			readingBody = false;
			if (ec) {
				if (!forcedShutdown(ec))
					std::cout << "readBody error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
			} else {
				MessageBuffer copy = *m_buffer;
				
				copy.noDelete = true;
				if (m_server) {
					if (m_server->m_onReceive) {
						m_server->m_onReceive(copy, m_uuid);
						m_buffer->flush();
					}
				}
				if (m_client) {
					if (m_client->m_onReceive) {
						m_client->m_onReceive(copy);
						m_buffer->flush();
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
	void Connection::writeBody() {
		if (!m_socket.is_open())
			return;
		writingBody = true;
		
		asio::async_write(m_socket, asio::buffer(m_outMessages.front()->m_data + MessageBuffer::HEAD_SIZE, m_outMessages.front()->size()-MessageBuffer::HEAD_SIZE),
			[this](std::error_code ec, std::size_t length) {
				writingBody = false;
				if (ec) {
					if (!forcedShutdown(ec))
						std::cout << "writeBody error "<< ec.value()<<" " << m_uuid << " " << ec.message() << "\n";
				} else {

					m_outMessages.erase(m_outMessages.begin());

					if (m_outMessages.size() > 0) {
						writeHead();
					}
				}
			});
	}
}