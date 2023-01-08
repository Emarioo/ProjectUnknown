#pragma once

#include "ProUnk/World/Dimension.h"

#include "ProUnk/Registries/InventoryRegistry.h"
#include "ProUnk/Registries/ObjectInfoRegistry.h"
#include "ProUnk/Registries/ComplexDataRegistry.h"
#include "ProUnk/Registries/ModelRegistry.h"

namespace prounk {
	
	enum NetCommand : int {
		MoveObject,
		AddObject,
		DELETE_OBJECT,
		AnimateObject,
		EditObject,
		DamageObject,
	};
	const char* to_string(NetCommand value);
	engone::Logger& operator<<(engone::Logger& log, NetCommand value);

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
	// Amazing name for an amazing class or interface
	class NetworkFuncs {
	public:
		NetworkFuncs() = default;

		void netMoveObject(engone::EngineObject* object);
		//void netMoveObject(engone::UUID uuid, const rp3d::Transform& transform, const rp3d::Vector3& velocity, const rp3d::Vector3& angular);
		//void netAddObject(engone::UUID uuid, int objectType, const std::string& modelAsset);
		void netAddObject(engone::UUID uuid, engone::EngineObject* object);
		void netDeleteObject(engone::UUID uuid);
		void netAnimateObject(engone::UUID uuid, const std::string& instance, const std::string& animation, bool loop, float speed, float blend, float frame);

		// separate edit commands into multilpe commands. not sub types of edit commands.
		void netEditObject(engone::UUID uuid, int type, uint64_t data);
		void netEditCombatData(engone::UUID uuid, engone::UUID from, bool yes);
		void netDamageObject(engone::UUID uuid, float damage);

		virtual engone::Client& getClient() = 0;
		virtual engone::Server& getServer() = 0;

		bool networkRunning();
		void networkSend(engone::MessageBuffer& msg);

		Session* getSession();

	private:

		//Session* m_session = nullptr;
	
		friend class Session;
	};

	class GameApp;
	class Session : public NetworkFuncs {
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
		engone::Client& getClient() override;
		engone::Server& getServer() override;

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