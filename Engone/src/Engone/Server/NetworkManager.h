#pragma once

#include "asio.hpp"

namespace engone
{
	enum class ConType
	{
		Server,
		Client
	};

	class Connection
	{
	public:
		Connection()=default;
		Connection(asio::ip::tcp::socket socket);

		std::string ip;
		uint16_t port;
		uint32_t uuid;

		asio::ip::tcp::socket socket;

		void* object;

		template <class T>
		T* GetObject()
		{
			return reinterpret_cast<T>(object);
		}

	};

	class Server
	{
	public:
		Server();

		bool open = false;

		void SetConnect(std::function<bool()> onConnect);
		void SetReceive(std::function<void(void*, uint32_t)> onRecieve);
		/*
		onConnect and onReceive needs to be set first.
		*/
		void Start(const std::string& port);
		
		void Send(void* data,uint32_t size);

		void Update(uint32_t maxMessages = -1);

	private:

		asio::ip::tcp::acceptor acceptor;

		std::vector<Connection*> connections;

		std::vector<char> buffer;

		std::function<bool()> onConnect;
		std::function<void(void*, uint32_t)> onReceive;
		void OnConnect();
		void OnReceive(asio::ip::tcp::socket& socket);
	};
	class Client
	{
	public:
		Client();

		bool open=false;

		void SetReceive(std::function<void(void*, uint32_t)> onRecieve);

		// onReceive needs to be set first
		void Connect(const std::string& ip,uint16_t port);

		void Send(void* data, uint32_t size);

	private:
		asio::ip::tcp::socket socket;

		std::function<void(void*, uint32_t)> onReceive;
		void OnReceive(asio::ip::tcp::socket& socket);

		std::vector<char> buffer;
	};

	void InitNetwork();
	void UninitNetwork();

}