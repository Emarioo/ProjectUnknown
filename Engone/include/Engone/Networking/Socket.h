#pragma once

#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
//#include <ws2tcpip.h>
#include <windows.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#include "../consoleHandler.h"

//#include "threads.h"

//#pragma comment(lib,"Ws2_32.lib")

/*
Notes:
	There are probably a lot of bugs. Especially if sockets and threads stop working when they do.
	One error some where is going to cause more errors elsewhere. Error handling isn't great is what i'm trying to say.

*/

WSADATA global_wsaData;

#ifndef SERVER_MAX_CONNECTIONS
#define SERVER_MAX_CONNECTIONS 5
#endif

#define SOCKET_ACTIVE 		0xFFFFFFFF
#define SOCKET_QUIT 		0xFFFFFFFE
//#define SOCKET_RECEIVING 	0xFFFFFFFE
//#define SOCKET_ACCEPTING 	0xFFFFFFFF

struct SocketBuffer {
	int max_size;// size of buffer
	int head;// head write/read
	char* buffer;// pointer to buffer
};
typedef struct SocketBuffer SocketBuffer;
struct ClientSocket {
	SOCKET socket;
	//Thread receiver;
	int (*onReceive)(SocketBuffer*);
	int status;
};
typedef struct ClientSocket ClientSocket;

struct ServerSocket;
typedef struct ServerSocket ServerSocket;

struct ReceiverInfo {
	ServerSocket* server;
	SOCKET socket;
	//Thread receiver;
	int uuid;
};
typedef struct ReceiverInfo ReceiverInfo;
struct ServerSocket {
	SOCKET socket;
	//Thread acceptor;
	ReceiverInfo clients[SERVER_MAX_CONNECTIONS];
	int (*onReceive)(int, SocketBuffer*);
	int status;
};

#ifndef MAIN_C
void socket_init();
void socket_cleanup();

SocketBuffer socket_buffer_create(int bufferSize);
void socket_buffer_destroy(SocketBuffer* buf);
void socket_buffer_resize(SocketBuffer* buf, int size);
void socket_buffer_flush(SocketBuffer* buf);

void socket_buffer_pull(SocketBuffer* buf, void* buffer, int size);
int socket_buffer_pull_int(SocketBuffer* buf);
char* socket_buffer_pull_string(SocketBuffer* buf);

// returns address in buffer where data is stored. (this way you alter it after it has been pushed)
void* socket_buffer_push(SocketBuffer* buf, void* buffer, int size);
void* socket_buffer_push_int(SocketBuffer* buf, int num);
void* socket_buffer_push_string(SocketBuffer* buf, char* str);

int CreateUUID();

int socket_client_connect(ClientSocket* client, char* ip, char* port, int (*func)(SocketBuffer*));
int socket_client_send(ClientSocket* client, SocketBuffer* buf);
void socket_client_wait(ClientSocket* client);
int socket_client_valid(ClientSocket* client);
void socket_client_disconnect(ClientSocket* client);

int socket_server_connect(ServerSocket* server, char* port, int (*func)(int, SocketBuffer*));
int server_receiver(void* data);
void socket_server_send(ServerSocket* server, SocketBuffer* buf, int uuid);
void socket_server_wait(ServerSocket* server);
int socket_server_valid(ServerSocket* server);
void socket_server_disconnect(ServerSocket* server);

#else
SocketBuffer socket_buffer_create(int bufferSize) {
	SocketBuffer buf = { bufferSize,0,malloc(bufferSize) };
	return buf;
}
void socket_buffer_destroy(SocketBuffer* buf) {
	free(buf->buffer);
}
void socket_buffer_flush(SocketBuffer* buf) {
	memset(buf->buffer, 0, buf->max_size);
	buf->head = 0;
}
void socket_buffer_resize(SocketBuffer* buf, int size) {
	if (buf->max_size >= size)
		return;

	buf->buffer = realloc(buf->buffer, size);
}
void socket_buffer_pull(SocketBuffer* buf, void* buffer, int size) {// size in bytes
	if (buf->max_size - buf->head < size)
		return;

	memcpy(buffer, &buf->buffer[buf->head], size);
	buf->head += size;
}
int socket_buffer_pull_int(SocketBuffer* buf) {// size in bytes
	// not enough bytes in buffer
	if (buf->max_size - buf->head < sizeof(int))
		return 0;

	buf->head += sizeof(int);

	return *((int*)&buf->buffer[buf->head - sizeof(int)]);
}
char* socket_buffer_pull_string(SocketBuffer* buf) {// size in bytes
	// not enough bytes in buffer
	if (buf->max_size - buf->head < 1)
		return 0;

	int length = strlen(&buf->buffer[buf->head]);
	buf->head += length + 1;

	return (char*)&buf->buffer[buf->head - (length + 1)];
}
void* socket_buffer_push(SocketBuffer* buf, void* buffer, int size) {// size in bytes
	if (buffer == NULL)
		return &buf->buffer[buf->head += size];

	if (buf->max_size - buf->head < size) {
		realloc(buf->buffer, buf->max_size + size);
		buf->max_size += size;
	}

	memcpy(&buf->buffer[buf->head], buffer, size);
	buf->head += size;
	return &buf->buffer[buf->head - size];
}
int* socket_buffer_push_int(SocketBuffer* buf, int num) {// size in bytes
	if (buf->max_size - buf->head < sizeof(int)) {
		realloc(buf->buffer, buf->max_size + sizeof(int));
		buf->max_size += sizeof(int);
	}

	memcpy(&buf->buffer[buf->head], &num, sizeof(int));
	buf->head += sizeof(int);

	return (int*)&buf->buffer[buf->head - sizeof(int)];
}
char* socket_buffer_push_string(SocketBuffer* buf, char* str) {// size in bytes
	int length = strlen(str);

	// make sure string is something reasonable. if not, the null terminated character might be missing
	//  and we would get a huge value which we don't want to allocate 
	//assert(length<500);

	if (buf->max_size - buf->head < length + 1) {
		realloc(buf->buffer, buf->max_size + length + 1);
		buf->max_size += length + 1;
	}

	memcpy(&buf->buffer[buf->head], str, length + 1);
	buf->head += length + 1;

	return (char*)&buf->buffer[buf->head - length - 1];
}

int CreateUUID() {// This is a really stupid way of creating uuid.
	while (1) {
		srand(time(NULL));
		int random = rand() | (rand() * 2 << 16);
		if (random != 0)
			return random;
	}
}
void socket_init() {
	if (WSAStartup(MAKEWORD(2, 2), &global_wsaData)) {
		//printf("Error with WSAStartup\n");
		return;
	}
}
int client_receiver(void* data) {
	ClientSocket* client = (ClientSocket*)data;
	const int maxBufferSize = 512;
	char buffer[maxBufferSize];

	//console_log("primed");
	while (client->status & (SOCKET_ACTIVE)) {
		int readBytes = recv(client->socket, buffer, maxBufferSize, 0);
		if (client->socket == INVALID_SOCKET) {
			console_log("recv disconnected");
			break;
		}
		//console_log("recv %d",readBytes);
		if (readBytes > 0) {
			SocketBuffer buf = { readBytes, 0, buffer };
			if (client->onReceive != NULL) {
				int ret = client->onReceive(&buf);
				if (ret == SOCKET_QUIT) {
					break;
				}
			} else {
				// receiver is null
			}
		} else if (readBytes == 0) {
			// ?
			break;
		} else {
			DWORD err = WSAGetLastError();

			if (err == WSAECONNRESET) {
				// Hard shutdown
				break;
			}

			LPTSTR Error = 0;
			err = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, (LPTSTR)&Error, 0, NULL);
			if (err != 0) {
				console_log(Error);
				free(ERROR);
			}

			return 1;
		}
	}
	int msg = SOCKET_QUIT;
	SocketBuffer buf = { sizeof(int),0,&msg };
	if (client->onReceive != NULL) {
		client->onReceive(&buf);
	}
	if (client->socket != INVALID_SOCKET) {
		closesocket(client->socket);
	}
	return 0;
}
int socket_client_connect(ClientSocket* client, char* ip, char* port, int (*func)(SocketBuffer*)) {
	socket_client_disconnect(client);

	client->onReceive = func;

	int error;
	struct addrinfo* result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	error = getaddrinfo(ip, port, &hints, &result);
	if (error != 0) {
		// Error getaddr
		return 1;
	}

	client->socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (client->socket == INVALID_SOCKET) {
		// Error with socket
		freeaddrinfo(result);
		return 1;
	}

	error = connect(client->socket, result->ai_addr, (int)result->ai_addrlen);
	freeaddrinfo(result);
	if (error == SOCKET_ERROR) {
		// Error with client connect
		closesocket(client->socket);
		return 1;
	}

	client->status = SOCKET_ACTIVE;
	thread_create(&client->receiver, client_receiver, client);

	//console_log("connected");

	return 0;
}
int socket_client_send(ClientSocket* client, SocketBuffer* buf) {
	if (socket_client_valid(client)) {
		int error = send(client->socket, buf->buffer, buf->head, 0);
		if (error == SOCKET_ERROR) {
			//printf("Error with client send\n");
			closesocket(client->socket);
			//WSACleanup();
			return 1;
		}
		//console_log("Send c");
	}
	return 0;
}
void socket_client_wait(ClientSocket* client) {
	thread_join(&client->receiver);
}
int socket_client_valid(ClientSocket* client) {
	return (client->status & SOCKET_ACTIVE);
}
void socket_client_disconnect(ClientSocket* client) {
	client->status = 0;

	if (client->socket != INVALID_SOCKET) {
		closesocket(client->socket);
	}
	if (thread_valid(&client->receiver))
		thread_join(&client->receiver);
}
int server_receiver(void* data) {
	ReceiverInfo* info = (ReceiverInfo*)data;

	const int maxBufferSize = 512;
	char buffer[maxBufferSize];

	//console_log("primed");
	while (info->server->status & SOCKET_ACTIVE) {
		int readBytes = recv(info->socket, buffer, maxBufferSize, 0);
		//console_log("recv %d",readBytes);
		if (info->socket == INVALID_SOCKET) {
			//printf("Error server receive socket"); // socket might have been destroyed or something
			break;
		}
		if (readBytes > 0) {
			SocketBuffer buf = { readBytes, 0, buffer };
			if (info->server->onReceive != NULL) {
				int ret = info->server->onReceive(info->uuid, &buf);
				if (ret == SOCKET_QUIT) {
					break;
				}
			} else {
				// receiver is null
			}
		} else if (readBytes == 0) {
			break;
		} else {
			DWORD err = WSAGetLastError();

			if (err == WSAECONNRESET) {
				// Shutdown from peer
				break;
			} else if (err == WSAECONNABORTED) {
				// Shutdown from program
				break;
			}

			LPTSTR Error = 0;
			DWORD fErr = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, (LPTSTR)&Error, 0, NULL);
			if (fErr != 0) {
				console_log("%d %s", err, Error);
				free(ERROR);
			}

			break;
		}
	}

	int msg = SOCKET_QUIT;
	SocketBuffer buf = { sizeof(int),0,&msg };
	if (info->server->onReceive != NULL) {
		info->server->onReceive(info->uuid, &buf);
	}

	info->uuid = 0;
}
int server_acceptor(void* data) {
	ServerSocket* server = (ServerSocket*)data;
	while (server->status & SOCKET_ACTIVE) {
		SOCKET client = accept(server->socket, NULL, NULL);

		if (client == INVALID_SOCKET) {
			//  Usually means the server socket was closed
			break;
		}

		int index = -1;
		for (int i = 0; i < SERVER_MAX_CONNECTIONS; i++) {
			if (server->clients[i].uuid == 0) {
				index = i;
			}
		}

		if (index == -1) {
			// No available thread
			closesocket(client);
			continue;
		}
		// join incase it was used before
		thread_join(&server->clients[index].receiver);
		server->clients[index].socket = client;
		server->clients[index].uuid = CreateUUID();
		thread_create(&server->clients[index].receiver, server_receiver, &server->clients[index]);
	}

	server->status = 0;
	if (server->socket != INVALID_SOCKET) {
		closesocket(server->socket);
	}

	// joining all threads
	for (int i = 0; i < SERVER_MAX_CONNECTIONS; i++) {
		if (server->clients[i].uuid != 0) {
			if (server->clients[i].socket != INVALID_SOCKET) {
				closesocket(server->clients[i].socket);
			}
			thread_join(&server->clients[i].receiver);
		}
	}
}
int socket_server_connect(ServerSocket* server, char* port, int(*func)(int, SocketBuffer*)) {
	socket_server_disconnect(server);

	server->onReceive = func;

	int error;
	server->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server->socket == INVALID_SOCKET) {
		//printf("Error with server socket\n");
		//WSACleanup();
		return 1;
	}

	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");

	// check if atoi or port is fine
	int num = atoi(port);
	service.sin_port = htons(num);

	error = bind(server->socket, (SOCKADDR*)&service, sizeof(service));
	if (error == SOCKET_ERROR) {
		//printf("Error with server bind\n");
		closesocket(server->socket);
		//WSACleanup();
		return 1;
	}
	error = listen(server->socket, SERVER_MAX_CONNECTIONS);
	if (error == SOCKET_ERROR) {
		//printf("Error with server listen\n");
		closesocket(server->socket);
		//WSACleanup();
		return 1;
	}
	server->status = SOCKET_ACTIVE;
	thread_create(&server->acceptor, server_acceptor, server);

	return 0;
}
void socket_server_send(ServerSocket* server, SocketBuffer* buf, int uuid) {
	int index = -1;
	if (uuid != 0) {
		for (int i = 0; i < SERVER_MAX_CONNECTIONS; i++) {
			if (server->clients[i].uuid == uuid) {
				index = i;
				break;
			}
		}
	}
	//console_log("sent1 %d",index);
	if (index == -1) {
		for (int i = 0; i < SERVER_MAX_CONNECTIONS; i++) {
			if (server->clients[i].uuid != 0) {
				int error = send(server->clients[i].socket, buf->buffer, buf->head, 0);
				if (error == SOCKET_ERROR) {
					//console_log("Error server send");
					//closesocket(server->clients[index].socket);
					//return 1;
				} else {
					//console_log("0 uuid send");
				}
			}
		}
	} else {
		int error = send(server->clients[index].socket, buf->buffer, buf->head, 0);
		if (error == SOCKET_ERROR) {
			//console_log("Error server send");
			//closesocket(server->clients[index].socket);
			//return 1;
		} else {
			//console_log("uuid send");
		}
	}
}
void socket_server_wait(ServerSocket* server) {
	thread_join(&server->acceptor);
}
int socket_server_valid(ServerSocket* server) {
	return (server->status & SOCKET_ACTIVE);
}
void socket_server_disconnect(ServerSocket* server) {
	server->status = 0;

	if (server->socket != INVALID_SOCKET) {
		closesocket(server->socket);
	}
	thread_join(&server->acceptor);

	for (int i = 0; i < SERVER_MAX_CONNECTIONS; i++) {// maybe try to disconnect first?
		server->clients[i].server = server;
		server->clients[i].uuid = 0;
	}
}
void socket_cleanup() {
	WSACleanup();
}
#endif