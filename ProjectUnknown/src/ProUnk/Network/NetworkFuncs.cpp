
#include "ProUnk/Network/NetworkFuncs.h"

#include "ProUnk/World/Session.h"

#include "ProUnk/GameApp.h"

namespace prounk {
	const char* to_string(NetCommand value) {
		switch (value) {
		case NET_ADD_TERRAIN: return "NET_ADD_TERRAIN";
		case NET_ADD_ITEM: return "NET_ADD_ITEM";
		case NET_ADD_WEAPON: return "NET_ADD_WEAPON";
		case NET_ADD_CREATURE: return "NET_ADD_CREATURE";
		case NET_MOVE: return "NET_MOVE";
		case NET_DELETE: return "NET_DELETE";
		case NET_DAMAGE: return "NET_DAMAGE";
		case NET_SET_HEALTH: return "NET_SET_HEALTH";
		case NET_ANIMATE: return "NET_ANIMATE";
		case NET_TRIGGER: return "NET_TRIGGER";
		case NET_EDIT: return "NET_EDIT";
		case NET_EDIT_BODY: return "NET_EDIT_BODY";
		}
		return "Unknown";
	}
	engone::Logger& operator<<(engone::Logger& log, NetCommand value) {
		return log << to_string(value);
	}
	Session* NetworkFuncs::getSession() {
		return (Session*)this; // this is dangerous
	}
	void NetworkFuncs::netMoveObject(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::netMoveObject - neither server or client is running\n";
			return;
		}

		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = NET_MOVE;
		msg.push(cmd);
		pushObject(msg,object);

		rp3d::Vector3 vel = object->getRigidBody()->getLinearVelocity();
		rp3d::Vector3 angVel = object->getRigidBody()->getAngularVelocity();
		msg.push(&object->getRigidBody()->getTransform());
		msg.push(&vel);
		msg.push(&angVel);

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netAddGeneral(engone::EngineObject* object) {
		using namespace engone;
		//log::out << "netAddGeneral " << (BasicObjectType)object->getObjectType() << "\n";
		if (object->getObjectType() & OBJECT_TERRAIN) {
			netAddTerrain(object);
		}else if (object->getObjectType() & OBJECT_ITEM) {
			netAddItem(object);
		} else if (object->getObjectType() & OBJECT_WEAPON) {
			netAddWeapon(object);
		}else if (object->getObjectType() & OBJECT_CREATURE) {
			netAddCreature(object);
		}
	}
	void NetworkFuncs::netAddTerrain(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		log::out << "netAddGeneral " << (BasicObjectType)object->getObjectType() << "\n";

		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = NET_ADD_TERRAIN;
		msg.push(cmd);
		pushObject(msg, object);

		msg.push(object->getModel()->getLoadName());

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netAddItem(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		log::out << "netAddGeneral " << (BasicObjectType)object->getObjectType() << "\n";

		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = NET_ADD_ITEM;
		msg.push(cmd);
		pushObject(msg, object);

		Session* session = getSession();

		auto& info = session->objectInfoRegistry.getItemInfo(object->getObjectInfo());

		ItemType type = info.item.getType();
		//ModelId id = info.item.getModelId();
		int count = info.item.getCount();
		//info.item.getDisplayName();
		//info.item.getComplexData();
		msg.push(type);
		msg.push(count);

		auto& list = info.item.getComplexData()->getList();
		int propCount = list.size();
		
		msg.push(&propCount);
		msg.push(list.data(), propCount);
		
		//for (int i = 0; i < propCount;i++) {
		//	msg.push(&list[i].prop);
		//	msg.push(&list[i].value);
		//}

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netAddWeapon(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		log::out << "netAddGeneral " << (BasicObjectType)object->getObjectType() << "\n";

		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = NET_ADD_WEAPON;
		msg.push(cmd);
		pushObject(msg, object);

		Session* session = getSession();

		//auto& info = session->objectInfoRegistry.getWeaponInfo(object->getObjectInfo());
		
		msg.push(object->getModel()->getLoadName());

		// skipping combat data for now

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netAddCreature(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		log::out << "netAddGeneral " << (BasicObjectType)object->getObjectType() << "\n";

		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = NET_ADD_CREATURE;
		msg.push(cmd);
		pushObject(msg, object);

		int objectType=object->getObjectType(); // could be OBJECT_PLAYER or OBJECT_DUMMY and not just player
		msg.push(&objectType);
		Session* session = getSession();

		auto& info = session->objectInfoRegistry.getCreatureInfo(object->getObjectInfo());
		
		msg.push(object->getModel()->getLoadName());

		// skipping inventory and combat data

		//-- Send message
		networkSend(msg);
	}
	void  NetworkFuncs::netDeleteObject(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = NET_DELETE;
		msg.push(cmd);
		pushObject(msg, object);
		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netAnimateObject(engone::EngineObject* object, const std::string& instance, const std::string& animation, bool loop, float speed, float blend, float frame) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::netAnimateObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = NET_ANIMATE;
		msg.push(cmd);
		pushObject(msg, object);

		msg.push(instance);
		msg.push(animation);
		msg.push(&loop);
		msg.push(&speed);
		msg.push(&blend);
		msg.push(&frame);

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netEditObject(engone::EngineObject* object, int type, uint64_t data) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::editObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = NET_EDIT;
		msg.push(cmd);
		pushObject(msg, object);

		msg.push(&type);
		msg.push(&data);

		//-- Send message
		networkSend(msg);
	}
	//void NetworkFuncs::netEditCombatData(engone::EngineObject* object, engone::UUID player, bool yes) {
	//	using namespace engone;
	//	if (!networkRunning()) {
	//		//log::out << "World::editCombatObject - neither server or client is running\n";
	//		return;
	//	}
	//	//-- Prepare message
	//	MessageBuffer msg;
	//	NetCommand cmd = EditObject;
	//	msg.push(cmd);
	//	pushObject(msg, object);

	//	int type = 2;
	//	msg.push(&type);

	//	msg.push(&player);
	//	msg.push(&yes);

	//	//-- Send message
	//	networkSend(msg);
	//}
	void NetworkFuncs::netDamageObject(engone::EngineObject* object, float damage) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "Playground::moveObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = NET_DAMAGE;
		msg.push(cmd);
		pushObject(msg, object);

		msg.push(&damage);

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netSetHealth(engone::EngineObject* object, float health) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "Playground::moveObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = NET_SET_HEALTH;
		msg.push(cmd);
		pushObject(msg, object);

		msg.push(&health);

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netSetTrigger(engone::EngineObject* object, bool yes) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "Playground::moveObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = NET_TRIGGER;
		msg.push(cmd);
		pushObject(msg, object);

		msg.push(&yes);

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::recAddTerrain(engone::MessageBuffer* msg, engone::UUID clientUUID) {
		using namespace engone;
		Session* session = getSession();
		
		//-- Extract data
		UUID_DIM obj;
		pullObject(msg, obj); // Issue: what if uuid already exists (the client may have sent the same uuid by intention)
		
		std::string modelAsset;
		msg->pull(modelAsset);

		//-- don't create object if uuid exists
		if (!obj.dim)
			return;

		EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);
		if (object) {
			obj.dim->getWorld()->releaseAccess(obj.uuid);
			log::out << log::RED << "NetworkFuncs : Cannot add " << obj.uuid << " because it exists\n";
			return;
		}
		EngineObject* newObject = CreateTerrain(obj.dim, obj.uuid);
		if(!newObject){
			log::out << log::RED << "NetworkFuncs : Failed creating " << obj.uuid << "\n";
			return;
		}
		if (clientUUID != 0) {
			auto find = session->getClientData().find(clientUUID);
			if (find == session->getClientData().end()) {
				log::out << log::RED << "NetworkFuncs : Could not find client data for " << clientUUID << "\n";
			} else {
				find->second.ownedObjects.push_back(newObject);
			}
		}
		obj.dim->getWorld()->releaseAccess(obj.uuid);
	}
	void NetworkFuncs::recAddItem(engone::MessageBuffer* msg, engone::UUID clientUUID){
		using namespace engone;
		Session* session = getSession();

		//-- Extract data
		UUID_DIM obj;
		pullObject(msg, obj); // Issue: what if uuid already exists (the client may have sent the same uuid by intention)

		ItemType type;
		int count;
		msg->pull(&type);
		msg->pull(&count);

		int propCount;
		msg->pull(&propCount);
		ComplexData temp;
		temp.getList().resize(propCount);
		msg->pull(temp.getList().data(), propCount);

		//-- don't create object if uuid exists
		if (!obj.dim)
			return;

		EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);
		obj.dim->getWorld()->releaseAccess(obj.uuid);
		if (object) {
			log::out << log::RED << "NetworkFuncs::recAddItem : Cannot add " << obj.uuid << " because it exists\n";
			return;
		}
		Item item = { type, count };
		item.getComplexData()->getList() = std::move(temp.getList()); // move the complex data

		EngineObject* newObject = CreateItem(obj.dim, item, obj.uuid);
		if (!newObject) {
			log::out << log::RED << "NetworkFuncs : Failed creating " << obj.uuid << "\n";
			return;
		}
		if (clientUUID != 0) {
			auto find = session->getClientData().find(clientUUID);
			if (find == session->getClientData().end()) {
				log::out << log::RED << "NetworkFuncs : Could not find client data for " << clientUUID << "\n";
			} else {
				find->second.ownedObjects.push_back(newObject);
			}
		}
		obj.dim->getWorld()->releaseAccess(obj.uuid);
	}
	void NetworkFuncs::recAddWeapon(engone::MessageBuffer* msg, engone::UUID clientUUID){
		using namespace engone;
		Session* session = getSession();

		//-- Extract data
		UUID_DIM obj;
		pullObject(msg, obj); // Issue: what if uuid already exists (the client may have sent the same uuid by intention)

		std::string modelAsset;
		msg->pull(modelAsset);

		//-- don't create object if uuid exists
		if (!obj.dim)
			return;

		EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);
		obj.dim->getWorld()->releaseAccess(obj.uuid);
		if (object) {
			log::out << log::RED << "NetworkFuncs::recAddWeapon : Cannot add " << obj.uuid << " because it exists\n";
			return;
		}

		EngineObject* newObject = CreateWeapon(obj.dim, modelAsset,obj.uuid);
		if (!newObject) {
			log::out << log::RED << "NetworkFuncs : Failed creating " << obj.uuid << "\n";
			return;
		}
		if (clientUUID != 0) {
			auto find = session->getClientData().find(clientUUID);
			if (find == session->getClientData().end()) {
				log::out << log::RED << "NetworkFuncs : Could not find client data for " << clientUUID << "\n";
			} else {
				//if (objectType == OBJECT_PLAYER)
				//	find->second.player = newObject;

				find->second.ownedObjects.push_back(newObject);
			}
		}
		obj.dim->getWorld()->releaseAccess(obj.uuid);
	}
	void NetworkFuncs::recAddCreature(engone::MessageBuffer* msg, engone::UUID clientUUID) {
		using namespace engone;
		Session* session = getSession();

		//-- Extract data
		UUID_DIM obj;
		pullObject(msg, obj); // Issue: what if uuid already exists (the client may have sent the same uuid by intention)

		int objectType;
		msg->pull(&objectType);

		std::string modelAsset;
		msg->pull(modelAsset);

		//-- don't create object if uuid exists
		if (!obj.dim)
			return;

		EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);
		obj.dim->getWorld()->releaseAccess(obj.uuid);
		if (object) {
			log::out << log::RED << "NetworkFuncs::recAddWeapon : Cannot add " << obj.uuid << " because it exists\n";
			return;
		}
		EngineObject* newObject = nullptr;
		if (objectType == OBJECT_PLAYER) {
			newObject=CreatePlayer(obj.dim, obj.uuid);
		} else if(objectType==OBJECT_DUMMY) {
			newObject = CreateDummy(obj.dim, obj.uuid);
		}
		if (!newObject) {
			log::out << log::RED << "NetworkFuncs : Failed creating " << obj.uuid << "\n";
			return;
		}
		if (clientUUID != 0) {
			auto find = session->getClientData().find(clientUUID);
			if (find == session->getClientData().end()) {
				log::out << log::RED << "NetworkFuncs : Could not find client data for " << clientUUID << "\n";
			} else {
				if (objectType == OBJECT_PLAYER)
					find->second.player = newObject;

				find->second.ownedObjects.push_back(newObject);
			}
		}
		obj.dim->getWorld()->releaseAccess(obj.uuid);
	}
	bool NetworkFuncs::networkRunning() {
		return (getClient().isRunning() || getServer().isRunning()) && getSession()->areMessagesEnabled();
	}
	void NetworkFuncs::networkSend(engone::MessageBuffer& msg) {
		if (getServer().isRunning())
			getServer().send(msg, 0, true);
		if (getClient().isRunning())
			getClient().send(msg);
	}
	void NetworkFuncs::pushObject(engone::MessageBuffer& msg, engone::EngineObject* obj) {
		using namespace engone;
		UUID uuid = obj->getUUID();
		msg.push(uuid);
		msg.push(((Dimension*)obj->getWorld()->getUserData())->getName());
	}
	void NetworkFuncs::pullObject(engone::MessageBuffer* msg, UUID_DIM& data) {
		msg->pull(&data.uuid);
		std::string temp;
		msg->pull(temp);
		data.dim = getSession()->getDimension(temp);
	}
}