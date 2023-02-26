
#include "ProUnk/Network/NetProtocols.h"

#include "ProUnk/World/Session.h"

#include "ProUnk/GameApp.h"

#include "ProUnk/Objects/BasicObjects.h"

namespace prounk {
	const char* to_string(NetMessageType value) {
		switch (value) {
		case NET_ADD_TERRAIN: return "NET_ADD_TERRAIN";
		case NET_ADD_TRIGGER: return "NET_ADD_TRIGGER";
		case NET_ADD_ITEM: return "NET_ADD_ITEM";
		case NET_ADD_WEAPON: return "NET_ADD_WEAPON";
		case NET_ADD_CREATURE: return "NET_ADD_CREATURE";
		case NET_MOVE: return "NET_MOVE";
		case NET_DELETE: return "NET_DELETE";
		
		case NET_DAMAGE: return "NET_DAMAGE";
		case NET_ANIMATE: return "NET_ANIMATE";

		case NET_EDIT_HEALTH: return "NET_EDIT_HEALTH";
		case NET_EDIT_TRIGGER: return "NET_EDIT_TRIGGER";
		case NET_EDIT_MODEL: return "NET_EDIT_MODEL";
		case NET_EDIT_BODY: return "NET_EDIT_BODY";
		}
		return "Unknown";
	}
	engone::Logger& operator<<(engone::Logger& log, NetMessageType value) {
		return log << to_string(value);
	}
	Session* NetProtocols::getSession() {
		return (Session*)this; // this is dangerous
	}
	void NetProtocols::netMoveObject(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::netMoveObject - neither server or client is running\n";
			return;
		}

		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_MOVE;
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
	void NetProtocols::netAddGeneral(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			return;
		}
		log::out << "netAddGeneral " << (BasicObjectType)object->getObjectType();
		//if (object->getModel())
		//	object->getModel()->getLoadName();
		log::out << "\n";

		if (IsObject(object,OBJECT_TERRAIN)) netAddTerrain(object);
		else if (IsObject(object,OBJECT_TRIGGER)) netAddTrigger(object);
		else if (IsObject(object, OBJECT_ITEM)) netAddItem(object);
		else if (IsObject(object, OBJECT_WEAPON))netAddWeapon(object);
		else if (IsObject(object, OBJECT_CREATURE)) netAddCreature(object);
		
	}
	void NetProtocols::netAddTerrain(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		//log::out << "netAddGeneral " << (BasicObjectType)object->getObjectType() << "" "\n";

		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_ADD_TERRAIN;
		msg.push(cmd);
		pushObject(msg, object);

		if(object->getModel())
			msg.push(object->getModel()->getLoadName());
		else {
			std::string oj;
			msg.push(oj);
		}
		//-- Send message
		networkSend(msg);
	}
	void NetProtocols::netAddTrigger(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << "netAddGeneral " << (BasicObjectType)object->getObjectType() << "" "\n";

		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_ADD_TRIGGER;
		msg.push(cmd);
		pushObject(msg, object);


		Session* session = getSession();
		//auto& info = session->objectInfoRegistry.getItemInfo(object->getObjectInfo());
		glm::vec3 size= { 5,5,5 };
		if (object->getRigidBody()->getNbColliders() != 0) {
			size = ToGlmVec3(((rp3d::BoxShape*)object->getRigidBody()->getCollider(0)->getCollisionShape())->getHalfExtents());
		} else {
			printf("trigger has no collider\n");
		}
		msg.push(size);
		//if (object->getModel())
		//	msg.push(object->getModel()->getLoadName());
		//else {
		//std::string oj;
		//msg.push(oj);
		//}
		//-- Send message
		networkSend(msg);
	}
	void NetProtocols::netAddItem(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		//log::out << "netAddGeneral " << (BasicObjectType)object->getObjectType() << "\n";

		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_ADD_ITEM;
		msg.push(cmd);
		pushObject(msg, object);

		Session* session = getSession();

		auto info = session->objectInfoRegistry.getItemInfo(object->getObjectInfo());

		ItemType type = info->item.getType();
		//ModelId id = info.item.getModelId();
		int count = info->item.getCount();
		//info.item.getDisplayName();
		//info.item.getComplexData();
		msg.push(type);
		msg.push(count);

		auto& list = info->item.getComplexData()->getList();
		int propCount = list.size();
		
		msg.push(&propCount);
		msg.push((ComplexData::Entry*)list.data(), propCount);
		
		//for (int i = 0; i < propCount;i++) {
		//	msg.push(&list[i].prop);
		//	msg.push(&list[i].value);
		//}

		//-- Send message
		networkSend(msg);
	}
	void NetProtocols::netAddWeapon(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		//log::out << "netAddGeneral " << (BasicObjectType)object->getObjectType() << "\n";

		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_ADD_WEAPON;
		msg.push(cmd);
		pushObject(msg, object);

		Session* session = getSession();

		//auto& info = session->objectInfoRegistry.getWeaponInfo(object->getObjectInfo());
		
		msg.push(object->getModel()->getLoadName());

		// skipping combat data for now

		//-- Send message
		networkSend(msg);
	}
	void NetProtocols::netAddCreature(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		//log::out << "netAddGeneral " << (BasicObjectType)object->getObjectType() << "\n";

		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_ADD_CREATURE;
		msg.push(cmd);
		pushObject(msg, object);

		int objectType=object->getObjectType(); // could be OBJECT_PLAYER or OBJECT_DUMMY and not just player
		msg.push(&objectType);
		Session* session = getSession();

		//auto info = session->objectInfoRegistry.getCreatureInfo(object->getObjectInfo());
		
		msg.push(object->getModel()->getLoadName());

		// skipping inventory and combat data

		//-- Send message
		networkSend(msg);
	}
	void  NetProtocols::netDeleteObject(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_DELETE;
		msg.push(cmd);
		pushObject(msg, object);
		//-- Send message
		networkSend(msg);
	}
	void NetProtocols::netAnimateObject(engone::EngineObject* object, const std::string& instance, const std::string& animation, bool loop, float speed, float blend, float frame) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::netAnimateObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_ANIMATE;
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

	void NetProtocols::netEditBody(engone::EngineObject* object, uint64_t bodyType) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::editObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_EDIT_BODY;
		msg.push(cmd);
		pushObject(msg, object);

		msg.push(&bodyType);

		//-- Send message
		networkSend(msg);
	}
	void NetProtocols::netEditTrigger(engone::EngineObject* object, bool yes) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::editObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_EDIT_TRIGGER;
		msg.push(cmd);
		pushObject(msg, object);

		msg.push(&yes);

		//-- Send message
		networkSend(msg);
	}
	void NetProtocols::netEditModel(engone::EngineObject* object, const std::string& modelName) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::editObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_EDIT_MODEL;
		msg.push(cmd);
		pushObject(msg, object);

		msg.push(modelName);

		//-- Send message
		networkSend(msg);
	}
	void NetProtocols::netEditHealth(engone::EngineObject* object, float health) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::editObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_EDIT_HEALTH;
		msg.push(cmd);
		pushObject(msg, object);

		msg.push(&health);

		//-- Send message
		networkSend(msg);
	}
	void NetProtocols::netDamageObject(engone::EngineObject* object, float damage, glm::vec3 knockDirection) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "Playground::moveObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetMessageType cmd = NET_DAMAGE;
		msg.push(cmd);
		pushObject(msg, object);

		msg.push(&damage);
		msg.push(&knockDirection);

		//-- Send message
		networkSend(msg);
	}
	void NetProtocols::recDamageObject(engone::MessageBuffer* msg, engone::UUID clientUUID) {
		using namespace engone;
		//-- Extract data
		UUID_DIM obj;
		pullObject(msg, obj);

		float damage;
		msg->pull(&damage);

		glm::vec3 knockDirection;
		msg->pull(&knockDirection);

		//-- Find object with matching UUID
		if (obj.dim) {
			EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);

			//log::out << "Session::Receive : DamageObject is broken\n";
			if (object) {

				auto oinfo = getSession()->objectInfoRegistry.getCreatureInfo(object->getObjectInfo());
				oinfo->combatData.setHealth(oinfo->combatData.getHealth()-damage);
				if (oinfo->combatData.getHealth() < 0)
					oinfo->combatData.setHealth(0);

				object->applyForce(knockDirection);

				// Todo: Could dealCombat be changed a bit to call a sub function which this code also calls to avoid code duplication?

				// Todo: Change this to contact point instead of object.pos. Contact point should be added in NET_DAMAGE.
				getSession()->getParent()->visualEffects.addDamageParticle(object->getPosition());
				obj.dim->getWorld()->releaseAccess(obj.uuid);
			}
		}
	}
	void NetProtocols::recAddTerrain(engone::MessageBuffer* msg, engone::UUID clientUUID) {
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
			log::out << log::RED << "NetProtocols : Cannot add " << obj.uuid << " because it exists\n";
			return;
		}
		EngineObject* newObject = CreateTerrain(obj.dim, modelAsset, obj.uuid);
		if(!newObject){
			log::out << log::RED << "NetProtocols : Failed creating " << obj.uuid << "\n";
			return;
		}
		if (clientUUID != 0) {
			auto find = session->getClientData().find(clientUUID);
			if (find == session->getClientData().end()) {
				log::out << log::RED << "NetProtocols : Could not find client data for " << clientUUID << "\n";
			} else {
				find->second.ownedObjects.push_back(newObject);
			}
		}
		obj.dim->getWorld()->releaseAccess(obj.uuid);
	}
	void NetProtocols::recAddTrigger(engone::MessageBuffer* msg, engone::UUID clientUUID) {
		using namespace engone;
		Session* session = getSession();

		//-- Extract data
		UUID_DIM obj;
		pullObject(msg, obj); // Issue: what if uuid already exists (the client may have sent the same uuid by intention)

		glm::vec3 size;
		msg->pull(&size);

		//-- don't create object if uuid exists
		if (!obj.dim)
			return;

		EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);
		if (object) {
			obj.dim->getWorld()->releaseAccess(obj.uuid);
			log::out << log::RED << "NetProtocols : Cannot add " << obj.uuid << " because it exists\n";
			return;
		}
		EngineObject* newObject = CreateTrigger(obj.dim,size, obj.uuid);
		if (!newObject) {
			log::out << log::RED << "NetProtocols : Failed creating " << obj.uuid << "\n";
			return;
		}
		if (clientUUID != 0) {
			auto find = session->getClientData().find(clientUUID);
			if (find == session->getClientData().end()) {
				log::out << log::RED << "NetProtocols : Could not find client data for " << clientUUID << "\n";
			} else {
				find->second.ownedObjects.push_back(newObject);
			}
		}
		obj.dim->getWorld()->releaseAccess(obj.uuid);
	}
	void NetProtocols::recAddItem(engone::MessageBuffer* msg, engone::UUID clientUUID){
		using namespace engone;
		Session* session = getSession();

		//-- Extract data
		UUID_DIM obj{};
		pullObject(msg, obj); // Issue: what if uuid already exists (the client may have sent the same uuid by intention)

		ItemType type=0;
		int count=0;
		msg->pull(&type);
		msg->pull(&count);

		int propCount=0;
		msg->pull(&propCount);
		ComplexData temp{};
		temp.getList().resize(propCount);
		msg->pull((ComplexData::Entry*)temp.getList().data(), propCount);

		//-- don't create object if uuid exists
		if (!obj.dim)
			return;

		EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);
		obj.dim->getWorld()->releaseAccess(obj.uuid);
		if (object) {
			log::out << log::RED << "NetProtocols::recAddItem : Cannot add " << obj.uuid << " because it exists\n";
			return;
		}
		Item item = { type, count };
		item.getComplexData()->getList() = std::move(temp.getList()); // move the complex data

		EngineObject* newObject = CreateItem(obj.dim, item, obj.uuid);
		if (!newObject) {
			log::out << log::RED << "NetProtocols : Failed creating " << obj.uuid << "\n";
			return;
		}
		if (clientUUID != 0) {
			auto find = session->getClientData().find(clientUUID);
			if (find == session->getClientData().end()) {
				log::out << log::RED << "NetProtocols : Could not find client data for " << clientUUID << "\n";
			} else {
				find->second.ownedObjects.push_back(newObject);
			}
		}
		obj.dim->getWorld()->releaseAccess(obj.uuid);
	}
	void NetProtocols::recAddWeapon(engone::MessageBuffer* msg, engone::UUID clientUUID){
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
			log::out << log::RED << "NetProtocols::recAddWeapon : Cannot add " << obj.uuid << " because it exists\n";
			return;
		}

		EngineObject* newObject = CreateWeapon(obj.dim, modelAsset,obj.uuid);
		if (!newObject) {
			log::out << log::RED << "NetProtocols : Failed creating " << obj.uuid << "\n";
			return;
		}
		if (clientUUID != 0) {
			auto find = session->getClientData().find(clientUUID);
			if (find == session->getClientData().end()) {
				log::out << log::RED << "NetProtocols : Could not find client data for " << clientUUID << "\n";
			} else {
				//if (objectType == OBJECT_PLAYER)
				//	find->second.player = newObject;

				find->second.ownedObjects.push_back(newObject);
			}
		}
		obj.dim->getWorld()->releaseAccess(obj.uuid);
	}
	void NetProtocols::recAddCreature(engone::MessageBuffer* msg, engone::UUID clientUUID) {
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
			log::out << log::RED << "NetProtocols::recAddWeapon : Cannot add " << obj.uuid << " because it exists\n";
			return;
		}
		EngineObject* newObject = nullptr;
		if (objectType == OBJECT_PLAYER) {
			newObject=CreatePlayer(obj.dim, obj.uuid);
		} else if(objectType==OBJECT_DUMMY) {
			newObject = CreateDummy(obj.dim, obj.uuid);
		}
		if (!newObject) {
			log::out << log::RED << "NetProtocols : Failed creating " << obj.uuid << "\n";
			return;
		}
		if (clientUUID != 0) {
			auto find = session->getClientData().find(clientUUID);
			if (find == session->getClientData().end()) {
				log::out << log::RED << "NetProtocols : Could not find client data for " << clientUUID << "\n";
			} else {
				if (objectType == OBJECT_PLAYER)
					find->second.player = newObject;

				find->second.ownedObjects.push_back(newObject);
			}
		}
		obj.dim->getWorld()->releaseAccess(obj.uuid);
	}
	bool NetProtocols::networkRunning() {
		return (getClient().isRunning() || getServer().isRunning()) && getSession()->areMessagesEnabled();
	}
	void NetProtocols::networkSend(engone::MessageBuffer& msg) {
		if (getServer().isRunning())
			getServer().send(msg, 0, true);
		if (getClient().isRunning())
			getClient().send(msg);
	}
	void NetProtocols::pushObject(engone::MessageBuffer& msg, engone::EngineObject* obj) {
		using namespace engone;
		UUID uuid = obj->getUUID();
		msg.push(uuid);
		msg.push(((Dimension*)obj->getWorld()->getUserData())->getName());
	}
	void NetProtocols::pullObject(engone::MessageBuffer* msg, UUID_DIM& data) {
		msg->pull(&data.uuid);
		std::string temp;
		msg->pull(temp);
		data.dim = getSession()->getDimension(temp);
	}
}