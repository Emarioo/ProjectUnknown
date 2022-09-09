/*
What do i want and why do i want it
I want a way of sharing certain game wobjects between clients.
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
Update gameObject's data.


ISSUE: playground can have both a server and a client at the same time. This is unwanted.
*/

#pragma once

#include "Engone/GameObject.h"
#include "Engone/Networking/NetworkModule.h"
#include "Engone/ParticleModule.h"

namespace engone {
	// contains players, worlds, monsters
	// two types, one which updates objects, one which talks with the main
	// filter what should be specific and not specific

	enum ObjectFlag : uint8_t {
		ObjectShared=1,
		//ShareRender=2, // everyone else can see it.
	};
	typedef int ObjectFlags;
	struct SharedObject {
		ObjectFlags flags=0;
		GameObject* object=nullptr;
	};

	//class PermissionInfo {
	//public:
	//	
	//	bool has(UUID uuid) {
	//		for (UUID& id : objects) {
	//			if (id == uuid)
	//				return whitelist;
	//		}
	//		return !whitelist;
	//	}

	//	bool whitelist=false;
	//	// objects may work for more things than UUIDs for objects.
	//	std::vector<UUID> objects; // binary tree is an option instead of vector. uses more data though. Would be better if we are checking 1000 UUIDs. But there is probably going to be about 3.

	//};
	//

	//// for one client
	//class ClientPermissions {
	//public:
	//	~ClientPermissions();

	//	bool has(PGCommand command) {
	//		for (auto[cmd,perm] : permissions) {
	//			if (cmd == command)
	//				return whitelist;
	//		}
	//		return !whitelist;
	//	}

	//	bool whitelist=false;
	//	std::unordered_map<PGCommand, PermissionInfo*> permissions;

	//};

	class Application;
	class GameGround {
	public:
		GameGround() = default;
		GameGround(Application* app);
		virtual ~GameGround();
		virtual void cleanup();

		//void addObject(GameObject* object,SharedFlags flags);
		void addParticleGroup(ParticleGroupT* group);
		void addObject(GameObject* object);
		//void removeObject(GameObject* object);

		//SharedObject& getObject(uint64_t uuid);

		inline std::vector<SharedObject>& getObjects() { return m_objects; };
		inline std::vector<ParticleGroupT*>& getParticleGroups() { return m_particleGroups; };

		void update(UpdateInfo& info);

#ifdef ENGONE_PHYSICS
		rp3d::PhysicsWorld* m_pWorld = nullptr;
		rp3d::PhysicsCommon* m_pCommon = nullptr;
#endif

	protected:
		std::vector<SharedObject> m_objects;
		std::vector<ParticleGroupT*> m_particleGroups;

		Application* m_app=nullptr;
	};
}

