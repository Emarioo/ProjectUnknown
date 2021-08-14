#include "Server.h"

#include "ExtraFunc.h"
#include "World/World.h"

/*
Server startup
  Load World_1
  Wait for connection

Networking
  Client connect to server
  Client send "LOGIN"
    Data: Username, Password
    If User exist
      Server send "STARTUP"
      Data: Player_Info(skill, location(world,xyz), items), World_Data(seed, settings)

Network Actions - FIX: SAFE DATA TRANSFER WITH ENCRYPTION
  Client connect Server : Nothing encryption?
  Client send  If user exist ->
    Server to Client
      Data : World Seed, Entities, Players

  Client send "player location" : If player is at new location send Other objects.
  
PLAYERPOS : send all players positions to all clients IF they have changed
ENTITYPOS : Send entitys position and other actions they've done
WORLDEVENT : 

*/


void log(std::string s) {
	std::cout << s << std::endl;
}
Server::Server()
	: Running(true),Network(&Server::StartNetwork,this)
{
	ServerLoop();
	Network.join();
}
void Server::LoadWorld() {
	World w1("data/Kaishi.txt");
	Worlds["Kaishi"] = w1;
}
void Server::LoadPlayerData() {
	std::vector<std::string> data = ReadFileList("data/playerdata.txt");
	for (std::string line : data) {
		PlayerData* plr = new PlayerData(line); // FIX: creation new object but never deleting it?
		UserData[plr->name] = plr;
		std::cout << plr->name << " - " << plr->world << std::endl;
	}
}
void Server::ServerLoop(){

	// Load world 1
	LoadWorld();

	// Load Player data
	LoadPlayerData();
	
	while (Running) {
		// Update Entities?
		
		break;
	}
	
}
void Server::StartNetwork(){
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, "27015", &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}

	// Accept a client socket
	while (true) {
		log("Waiting for connection...");
		SOCKET clientSocket = accept(ListenSocket, NULL, NULL);
		std::cout << "Socket " << clientSocket << " Connected" << std::endl;
		if (clientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}
		unsigned int id = NewID();
		Clients[id] = clientSocket;
		Threads.push_back(std::thread(&Server::ClientCon,this, clientSocket,id));
		if (Threads.size() > (unsigned int)3) // Three total connections allowed
			break;
	}
	for (unsigned int i = 0; i < Threads.size(); i++) {
		Threads[i].join();
	}
	// Cleanup and server shutdown
	closesocket(ListenSocket);
	WSACleanup();
}
unsigned int Server::NewID() {
	return LAST_ID++;
}
void Server::ClientCon(SOCKET clientSocket,unsigned int ID){
	// Player Connected
	//SendData(ID,241,15);

	bool hasLogin = false;

	const int buflen = 512;
	char buffer[buflen];
	while (true) {
		int iResult = recv(clientSocket, buffer, buflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			std::vector<std::string> set = SplitString(((std::string)buffer).substr(0,iResult),"/-/");
			if (!hasLogin) {
				if (set[0] == "LOGIN") {
					if (Users[set[1]] == set[2]) {
						// User is logged in
						hasLogin = true;
						PlayerData* pdata = UserData[set[1]];
						World* wor = &Worlds[UserData[set[1]]->world];
						const char* sent= (
							"STARTUP/-/player/:/" + pdata->GetData()+
							"/-/world/:/name/w:/"+pdata->world+
							"/w-/seed/w:/"+std::to_string(wor->seed)).c_str();
						SendData(ID,sent, sizeof(sent));
						// FIX: Might not need to send player name
					}
				}
			}
			else {

			}
			/*
			// Echo the buffer back to the sender
			int sendResult = send(clientSocket, recvbuf, result, 0);
			if (sendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
				return;
			}
			printf("Bytes sent: %d\n", sendResult);
			*/
		}
		/*
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			//WSACleanup();
			return;
		}
		*/
	}
	// shutdown the connection
	int result = shutdown(clientSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		//WSACleanup();
		return;
	}
	closesocket(clientSocket);
	//WSACleanup();
}
void Server::SendDataAll(const char *data, unsigned int size){
	for(const auto& n : Clients){
		int result = send(n.second, data, size, 0);
		if (result == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(n.second);
			//WSACleanup();
			return;
		}
		printf("Bytes sent: %d\n", result);
	}
}
void Server::SendData(unsigned int id,const char *data, unsigned int size) {
	SOCKET socket = Clients[id];
	
	int result = send(socket, data, size, 0);
	if (result == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(socket);
		//WSACleanup();
		return;
	}
	printf("Bytes sent: %d\n", result);
}