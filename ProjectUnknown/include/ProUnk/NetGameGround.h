#pragma once

#include "Engone/World/GameGround.h"
#include "Engone/GameObject.h"

enum NetCommand : int {
	MoveObject,
	AddObject,
	SyncObjects,
};
const char* to_string(NetCommand value);
engone::Logger& operator<<(engone::Logger& log, NetCommand value);

class ClientData {
public:
	ClientData() = default;

	engone::GameObject* player = nullptr;
};
class NetGameGround : public engone::GameGround {
public:
	NetGameGround() = default;
	NetGameGround(engone::Application* app);
	~NetGameGround() { 
		cleanup();
	}
	void cleanup() override;

	//-- commands
	void netMoveObject(engone::UUID uuid, const rp3d::Transform& tr);
	void netAddObject(engone::UUID uuid, int objectType, const std::string& modelAsset);
	// synchronize playground with connection.
	void netSyncObjects();

	engone::Server& getServer() { return m_server; };
	engone::Client& getClient() { return m_client; };

private:
	std::unordered_map<engone::UUID, ClientData> m_clients; // for server
	engone::Server m_server;
	engone::Client m_client;

};