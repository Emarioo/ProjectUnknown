#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include <vector>
#include <unordered_map>
#include <thread>
#include <string>

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#include "FileHandler.h"
#include "World/World.h"
#include "Server/PlayerData.h"

class Server
{
public:
	Server();
	// Game
	void ServerLoop();
	bool Running;

	std::unordered_map<std::string, World> Worlds;

	void LoadPlayerData();
	void LoadWorld();

	// Login In
	std::unordered_map<std::string, std::string> Users;
	std::unordered_map<std::string, PlayerData*> UserData;

	// Networking
	std::thread Network;
	void StartNetwork();
	void ClientCon(SOCKET clientSocket,unsigned int ID);
	void SendData(unsigned int id, const char *data, unsigned int size);
	void SendDataAll(const char *data, unsigned int size);
	std::vector<std::thread> Threads;
	std::unordered_map<unsigned int,SOCKET> Clients;
	unsigned int LAST_ID;

private:
	unsigned int NewID();
};