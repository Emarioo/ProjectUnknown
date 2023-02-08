/*
What do i want and why do i want it
I want a way of sharing certain game objects between clients.
Some objects belong to the host and cannot be altered
Some objects belong to a specific client
Some objects are global and can be manipulate by anyone

This magical class should be able to switch from sharing and not sharing with one simple function call.

This would be my implementation of multiplayer within a game. (atleast the game i am working on now)
This should make things easier, and not so difficult to alter. A lot of stuff behind the scenes.
Otherwise i may need to come up with a bunch of network message types, when to send and so on.
This is an organized way of containing that code inside on class, possibly more.

Specifics:
Playground is the magical class.
Playground can be a server or a client. not both, not multiple.
	or should you be able to connect to multiple servers? No, here is why
	the only reason you may want to connect to multiple servers is incase you have a mmorpg and need instancing, being connected to multiple would speed up transitioning between worlds.
	I do not plan on making a mmorpg in this way. I don't plan on using instancing or communicating between multiple servers.

To render/update objects in the engine YOU NEED to supply a playground. You can supply a flag to the playground to ignore server stuff.
Objects in playground will be rendered and updated properly. Objects can also be added and removed.
When adding objects you can supply a flag. This will cause the object to be shared how you want. An "empty" flag will not use any multiplayer features.
If your playground is a client when adding an object, a request will be sent to see if you are allowed to add/remove objects.
The server playground can restrict other playgrounds requests to change the playground. mainly objects.

Terminology:
Connected servers and clients in a network are referred to as one connected playground. The goal is to have one playground which
everyone can participate in with respective permissions.

Flags
No flags means local object, no one else can see or access it.

SharedRender
Where are objects updated. How avoid cheating.

Net Messages
Update position,
Update EngineObject's data.


ISSUE: playground can have both a server and a client at the same time. This is unwanted.
*/

#pragma once

#include "Engone/World/EngineWorld.h"
//#include "Engone/EngineObject.h"

#include "ProUnk/Registries/EntityRegistry.h"
#include "ProUnk/Registries/InventoryRegistry.h"

namespace prounk {

	enum NetCommand : int {
		MoveObject,
		AddObject,
		DELETE_OBJECT,
		AnimateObject,
		EditObject,
		DamageObject,
		SyncObjects,
	};
	const char* to_string(NetCommand value);
	engone::Logger& operator<<(engone::Logger& log, NetCommand value);

	class ClientData {
	public:
		ClientData() = default;

		engone::EngineObject* player = nullptr;
		std::vector<engone::EngineObject*> ownedObjects;
	};
	class GameApp;
	class World : public engone::EngineWorld {
	public:
		static const int OBJECT_NETMOVE = 0x00010000;

		World() = default;
		World(engone::Application* app);
		~World() {
			cleanup();
		}
		void cleanup() override;

		//-- Network messages you can send
		// 
		// pass in EngineObject instead?
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
		// synchronize playground with connection.
		//void netSyncObjects();

		void update(engone::LoopInfo& info) override;

		engone::Server& getServer() { return m_server; };
		engone::Client& getClient() { return m_client; };

		uint32_t getTotalBytesSent();
		uint32_t getTotalBytesReceived();
		float getBytesSentPerSecond();
		float getBytesReceivedPerSecond();

		void updateSample(float delta);

		GameApp* getApp() { return (GameApp*)m_app; }

		EntityRegistry entityRegistry;
		InventoryRegistry inventoryRegistry;
		//ModelRegistry modelRegistry;
		//ItemTypeRegistry itemTypeRegistry;

		CombatData* getCombatData(engone::EngineObject* object);
		Inventory* getInventory(engone::EngineObject* object);

	private:

		std::unordered_map<engone::UUID, ClientData> m_clients; // for server
		engone::Server m_server;
		engone::Client m_client;

		static const int SAMPLE_COUNT=1;
		uint32_t sample_index = 0;
		uint32_t lastSent = 0;
		uint32_t lastReceived = 0;
		uint32_t sample_sent[SAMPLE_COUNT]{0};
		uint32_t sample_received[SAMPLE_COUNT]{0};
		float sample_time[SAMPLE_COUNT]{0};
		
		float sentPerSecond=0;
		float receivedPerSecond=0;
	};
}