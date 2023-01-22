#pragma once

#include "ProUnk/World/Dimension.h"

#include "ProUnk/Registries/InventoryRegistry.h"
#include "ProUnk/Registries/ObjectInfoRegistry.h"
#include "ProUnk/Registries/ComplexDataRegistry.h"
#include "ProUnk/Registries/ModelRegistry.h"

#include "ProUnk/Network/NetProtocols.h"

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
		: public NetProtocols 
	{
	public:
		static const int OBJECT_NETMOVE = 0x00010000;

		static const char* DEFAULT_PORT;

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
		engone::Client& getClient() override;
		engone::Server& getServer() override;
		
		void processMessage(engone::MessageBuffer* msg, engone::UUID uuid);
		void processEvent(bool isServer, engone::NetEvent e, engone::UUID uuid);
		void processMessages(); // does events as well
		struct NetMessage {
			NetMessage(bool isServer, engone::NetEvent type, engone::UUID& uuid) : isEvent(true), isServer(isServer), netEvent(type), uuid(uuid) {}
			NetMessage(engone::MessageBuffer* msg, engone::UUID& uuid) : isEvent(false), msg(msg), uuid(uuid) {}
			bool isEvent = false;
			union {
				struct {
					bool isServer;
					engone::NetEvent netEvent;
				};
				engone::MessageBuffer* msg = nullptr;
			};
			engone::UUID uuid = 0;
		};
		std::vector<NetMessage> m_messageQueue;
		engone::Mutex m_messageQueueMutex;


		std::unordered_map<engone::UUID, ClientData>& getClientData() { return m_clients; }

		// Todo: Seperate enable send messages and receive messages.
		void enableMessages();
		void disableMessages();
		bool areMessagesEnabled();

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

		bool m_areMessagesEnabled = false;

		GameApp* m_parent;
	};
}