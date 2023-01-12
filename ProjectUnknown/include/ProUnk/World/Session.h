#pragma once

#include "ProUnk/World/Dimension.h"

#include "ProUnk/Registries/InventoryRegistry.h"
#include "ProUnk/Registries/ObjectInfoRegistry.h"
#include "ProUnk/Registries/ComplexDataRegistry.h"
#include "ProUnk/Registries/ModelRegistry.h"

//#include "ProUnk/Network/NetworkFuncs.h"

namespace prounk {
	
	class ClientData {
	public:
		ClientData() = default;

		engone::EngineObject* player = nullptr;
		std::vector<engone::EngineObject*> ownedObjects;
	};

	class NetworkStats {
	public:
		static const int SAMPLE_COUNT = 1;

		NetworkStats() = default;

		float getBytesSentPerSecond();
		float getBytesReceivedPerSecond();

		// Todo: remove the parameter session. Or at least move it somehow because the functions still needs the information
		// from server and client.
		void updateSample(float delta, Session* session);

	private:
		uint32_t sample_index = 0;
		uint32_t lastSent = 0;
		uint32_t lastReceived = 0;
		uint32_t sample_sent[SAMPLE_COUNT]{ 0 };
		uint32_t sample_received[SAMPLE_COUNT]{ 0 };
		float sample_time[SAMPLE_COUNT]{ 0 };

		float sentPerSecond = 0;
		float receivedPerSecond = 0;
	};

	class GameApp;
	class Session 
		//: public NetworkFuncs 
	{
	public:
		static const int OBJECT_NETMOVE = 0x00010000;

		Session(GameApp* app);
		~Session();
		void cleanup();

		//void serialize();
		//void deserialize();

		InventoryRegistry inventoryRegistry;
		ItemTypeRegistry itemTypeRegistry;
		ComplexDataRegistry complexDataRegistry;
		
		ObjectInfoRegistry objectInfoRegistry;
		ModelRegistry modelRegistry;

		// name needs to be unique. nullptr is returned if not.
		Dimension* createDimension(const std::string& name);
		
		Dimension* getDimension(const std::string& name);

		std::vector<Dimension*>& getDimensions();

		GameApp* getParent();

		void update(engone::LoopInfo& info);

		//engone::Mutex m_uuidMapMutex;
		//std::unordered_map<engone::UUID, *> m_uuidMapping;

		//-- Network stuff
		//engone::Client& getClient() override;
		//engone::Server& getServer() override;
		engone::Client& getClient();
		engone::Server& getServer();

		std::unordered_map<engone::UUID, ClientData>& getClientData() { return m_clients; }

		uint32_t getTotalBytesSent();
		uint32_t getTotalBytesReceived();

		NetworkStats& getNetworkStats();

	private:

		std::vector<Dimension*> m_dimensions;

		//-- Used by Client
		engone::Client m_client;

		//-- Used by Server
		std::unordered_map<engone::UUID, ClientData> m_clients; // for server
		engone::Server m_server;
	
		NetworkStats m_networkStats;

		GameApp* m_parent;
	};
}