#include "NetworkHandler.h"

namespace engone {

	static asio::io_context io_context;
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
			std::cout << "stopped context" << "\n";
			});

	}
	uint32_t lastUUID = 0;
	uint32_t GenerateUUID() {
		return ++lastUUID;
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
			//std::cout << "forced " << this << " uuid " << m_uuid << "\n";

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
	void Connection::readBody() {
		if (!m_socket.is_open())
			return;
		readingBody = true;
		asio::async_read(m_socket, asio::buffer(m_buffer->m_data + MSG_HEADER_SIZE, m_buffer->size()),
			[this](std::error_code ec, std::size_t length) {
				readingBody = false;
				if (ec) {
					if (!forcedShutdown(ec))
						std::cout << "readBody error " << ec.value() << " " << m_uuid << " " << ec.message() << "\n";
				} else {

					if (m_server) {
						if (m_server->m_onReceive) {
							m_server->m_onReceive(m_uuid, *m_buffer);
							m_buffer->flush();
						}
					}
					if (m_client) {
						if (m_client->m_onReceive) {
							m_client->m_onReceive(*m_buffer);
							m_buffer->flush();
						}
					}
					readHead();
				}
			});
	}
	void Connection::writeBody() {
		if (!m_socket.is_open())
			return;
		writingBody = true;
		asio::async_write(m_socket, asio::buffer(m_outMessages.front()->m_data + MSG_HEADER_SIZE, m_outMessages.front()->size()),
			[this](std::error_code ec, std::size_t length) {
				writingBody = false;
				if (ec) {
					if (!forcedShutdown(ec))
						std::cout << "writeBody error "<< ec.value()<<" " << m_uuid << " " << ec.message() << "\n";
				} else {

					MessageBuffer* msg = m_outMessages.front();
					msg->unshare();
					m_outMessages.erase(m_outMessages.begin());
					// delete message on client or server
					if (m_client) {
						for (int i = 0; i < m_client->m_outMessages.size(); i++) {
							MessageBuffer* buf = m_client->m_outMessages[i];
							if (buf == msg) {
								if (!buf->isShared()) {
									delete buf;
									m_client->m_outMessages.erase(m_client->m_outMessages.begin() + i);
								}
								break;
							}
						}
					}
					if (m_server) {
						for (int i = 0; i < m_server->m_outMessages.size(); i++) {
							MessageBuffer* buf = m_server->m_outMessages[i];
							if (buf == msg) {
								if (!buf->isShared()) {
									delete buf;
									m_server->m_outMessages.erase(m_server->m_outMessages.begin() + i);
								}
								break;
							}
						}
					}

					if (m_outMessages.size() > 0) {
						writeHead();
					}
				}
			});
	}
}