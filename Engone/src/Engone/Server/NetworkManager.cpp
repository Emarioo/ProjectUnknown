#include "gonpch.h"

#include "NetworkManager.h"

#include <iostream>

namespace engone
{
	static asio::io_context io_context;
	static std::thread* thrContext;
	static bool io_context_initialized=false;
	static void InitIOContext()
	{
		if (!io_context_initialized) {
			io_context_initialized = true;
			thrContext = new std::thread([&]() { io_context.run(); });
		}
	}

	Connection::Connection(asio::ip::tcp::socket socket)
		: socket(std::move(socket))
	{

	}
	Server::Server() : acceptor(io_context)
	{

	}
	void Server::SetConnect(std::function<bool()> onConnect)
	{
		this->onConnect = onConnect;
	}
	void Server::OnConnect()
	{
		/*
		acceptor.async_accept([=](std::error_code ec, asio::ip::tcp::socket socket) {
			if (!ec) {
				std::cout << "connect " << socket.remote_endpoint() << "\n";

				bool accept = onConnect();
				if (accept) {
					Connection* con = new Connection(std::move(socket));

					connections.push_back(con);

					OnReceive(con->socket);
				}
			}
			else {
				std::cout << "error " << ec.message()<<" "<<ec.value() << "\n";
			}
			});
		*/
	}
	void Server::SetReceive(std::function<void(void*, uint32_t)> onReceive)
	{
		this->onReceive = onReceive;
	}
	void Server::OnReceive(asio::ip::tcp::socket& socket)
	{
		/*
		asio::async_read(socket,asio::buffer(buffer.data(), buffer.size()),
			[&](std::error_code ec, std::size_t length) {
				if (!ec) {
					//std::cout << "\n\nRead " << length << " bytes\n\n";
					for (int i = 0; i < length; i++) {
						//std::cout << vBuffer[i];
					}
					onReceive(buffer.data(), length);
					OnReceive(socket);
				}
			});
		*/
	}
	void Server::Start(const std::string& port)
	{
		buffer.reserve(1*1024);
		open = true;
		OnConnect();

		InitIOContext();
		
		std::cout << "Started?\n";
	}
	void Server::Send(void* data, uint32_t size)
	{

	}
	void Server::Update(uint32_t maxMessages)
	{

	}
	Client::Client()
		: socket(io_context)
	{

	}
	void Client::SetReceive(std::function<void(void*,uint32_t)> onReceive)
	{
		this->onReceive = onReceive;
	}
	void Client::OnReceive(asio::ip::tcp::socket& socket)
	{
		/*
		asio::async_read(socket, asio::buffer(buffer.data(), buffer.size()),
			[&](std::error_code ec, std::size_t length) {
				if (!ec) {
					//std::cout << "\n\nRead " << length << " bytes\n\n";
					for (int i = 0; i < length; i++) {
						//std::cout << vBuffer[i];
					}
					onReceive(buffer.data(), length);
					OnReceive(socket);
				}
				else {

				}
			});
		*/
	}
	void Client::Connect(const std::string& ip, uint16_t port)
	{
		buffer.reserve(1 * 1024);

		std::error_code ec;
		asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip, ec), port);
		/*
		asio::async_connect(socket, endpoint, [&](std::error_code ec, asio::ip::tcp::endpoint enpoint) {
			if (!ec) {
				std::cout << "connected\n";
				if (socket.is_open()) {
					open = true;
					OnReceive(socket);
				}
			}
			else {
				std::cout << "not connected\n";
			}
			});
		*/
		InitIOContext();
	}
	void Client::Send(void* data, uint32_t size)
	{
		std::error_code ec;
		//asio::write(socket, asio::buffer(data, size), ec);
	}

	void InitNetwork()
	{
		Server server;
		server.SetConnect([]() {

			return true;
			});
		server.SetReceive([](void* data, uint32_t size) {
			std::cout << "server receive\n";
			});
		server.Start("40001");

		Client client;
		client.SetReceive([](void* data, uint32_t size) {
			std::cout << "client receive\n";
			});
		client.Connect("localhost", 40001);

		std::string req =
			"Here is your data sir.";

		client.Send(req.data(), req.size());

		std::this_thread::sleep_for(std::chrono::milliseconds(20000));

		std::cin.get();
	}
	void UninitNetwork()
	{
		io_context.stop();
		if (thrContext->joinable()) thrContext->join();
	}
}