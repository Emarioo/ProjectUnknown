#include "ProUnk/NetGameGround.h"
#include "Engone/Application.h"
#include "ProUnk/GameApp.h"

const char* to_string(NetCommand value) {
	switch (value) {
	case MoveObject: return "MoveObject";
	case AddObject: return "AddObject";
	case SyncObjects: return "SyncObjects";
	}
	return "Unknown";
}
engone::Logger& operator<<(engone::Logger& log, NetCommand value) {
	return log << to_string(value);
}

NetGameGround::NetGameGround(engone::Application* app) : GameGround(app) {
	using namespace engone;
	auto onRecv = [this](MessageBuffer& msg, UUID uuid) {
		//log::out << "Receive: Size:" <<msg.size()<<" UUID: "<<uuid<<"\n";

		NetCommand cmd;
		msg.pull(&cmd);
		//log::out << " Cmd: " << cmd << "\n";

		if (cmd == MoveObject) {
			UUID uuid;
			msg.pull(&uuid);
			rp3d::Transform tr;
			msg.pull(&tr);
			bool found = false;
			for (int i = 0; i < m_objects.size(); i++) {
				GameObject* object = m_objects[i].object;
				if (object->getUUID() == uuid) {
					found = true;
					object->rigidBody->setTransform(tr);
					break;
				}
			}
		} else if (cmd == AddObject) {
			UUID uuid;
			msg.pull(&uuid); // ISSUE: what if uuid already exists (the client may have sent the same uuid by intention)

			int objectType;
			msg.pull(&objectType);

			std::string modelAsset;
			msg.pull(modelAsset);
			//if (objectType == 0) {
				GameObject* newObject = new GameObject(this, uuid);
				GameGround::addObject(newObject);
				rp3d::Transform tr;
				tr.setPosition({ 0,0,0 });
				newObject->rigidBody = m_pWorld->createRigidBody(tr);
				//newObject->setOnlyTrigger(true);
				newObject->rigidBody->enableGravity(false);

				newObject->modelAsset = m_app->getStorage()->load<ModelAsset>(modelAsset, AssetStorage::SoftLoad);
				newObject->loadColliders(this);
				if(objectType==0)
					m_clients[uuid].player = newObject;
			//}
		} else if (cmd == SyncObjects) {
			int count;
			msg.pull(&count);
			for (int j = 0; j < count; j++) {
				UUID uuid;
				msg.pull(&uuid);
				bool found = false;
				for (int i = 0; i < m_objects.size(); i++) {
					if (uuid == m_objects[i].object->getUUID()) {
						found = true;
						break;
					}
				}
			}
		}

		return true; // wont work on server
	};
	m_server.setOnEvent([this](NetEvent e, UUID uuid) {
		log::out << "Server "<<e<<"\n";

		if (e == NetEvent::Connect) {
			m_clients[uuid] = {};
			game::GameApp* app = (game::GameApp*)m_app;
			for (int i = 0; i < m_objects.size();i++) {
				GameObject* obj = m_objects[i].object;
				int type = 1;
				if (app->player == obj)
					type = 0;
				netAddObject(obj->getUUID(), type,obj->modelAsset->getLoadName());
				netMoveObject(obj->getUUID(), obj->rigidBody->getTransform());
			}
		} else if (e == NetEvent::Disconnect) {
			m_clients.erase(uuid); // uuid should exist. NetworkModule may have a bug if it doesn't.
		}

		return true;
	});
	m_client.setOnEvent([this](NetEvent e, UUID uuid) {
		log::out << "Client "<<e<<"\n";

		if (e == NetEvent::Connect) {
			game::GameApp* app = (game::GameApp*)m_app;
			if (app->player) {
				netAddObject(app->player->getUUID(), 0, app->player->modelAsset->getLoadName());
			}
		} else if (e == NetEvent::Disconnect) {
			
		}

		return true;
	});
	m_server.setOnReceive([this, onRecv](MessageBuffer& msg, UUID uuid) {
		//log::out << "Relay ";

		 //detect command.
		//NetCommand cmd;
		//msg.pull(&cmd);

		// check if you have permission.
		auto find = m_clients.find(uuid);
		if (find != m_clients.end()) {
			
			onRecv(msg, uuid);
			//if (m_clients.size() > 1) {
			//	MessageBuffer sendMsg = msg;
			//	m_server.send(sendMsg, uuid, true);
			//}

			//ClientPermissions& perms = find->second;
			//if (perms.has(cmd)) {

			//	bool allowed = false;
			//	// check command specific permissions
			//	if (cmd == MoveObject) {
			//		PermissionInfo* info = perms.permissions[cmd];
			//		if (info) {// should not be nullptr but just incase
			//			UUID uuid;
			//			msg.pull(&uuid);
			//			if (info->has(uuid)) {
			//				allowed = true;
			//			} else {
			//				log::out << "Playground - " << uuid << " missing permission\n";
			//			}
			//		} else {
			//			log::out << log::RED << "Playground - PermissionInfo was nullptr\n";
			//		}
			//	}

			//	if (allowed) {
			//		msg.moveReadHead(0);
			//		onRecv(msg, uuid);

			//		// relay message to other clients

			//		// don't need to send if there are no other connections
			//		if (m_clients.size() > 1) {
			//			MessageBuffer sendMsg = msg.copy();
			//			m_server.send(sendMsg, uuid, true);
			//		}
			//	}
			//} else {
			//	// client tries to do stuff without access. Nothing serious.
			//	//log::out << "Playground::m_server - client missing permission for " << cmd << " (" << uuid << ")\n";
			//	log::out << "Playground::m_server - " << uuid << " missing permission for " << cmd << "\n";
			//}
		} else {
			// no data found for client. This should not happen because data was made when client connected.
			log::out << log::RED << "Playground::m_server - no data for client!\n";
		}

		return true;
	});

	m_client.setOnReceive(onRecv);
}
void NetGameGround::cleanup() {
	GameGround::cleanup();
}
void NetGameGround::netSyncObjects() {
	using namespace engone;
	// When client connects, it requests uuids from server.
	// server sends them
	MessageBuffer msg;

	msg.push(SyncObjects);
	int count = 0;
	msg.push(count);
}
void NetGameGround::netMoveObject(engone::UUID uuid, const rp3d::Transform& tr) {
	using namespace engone;
	if (!m_server.isRunning() && !m_client.isRunning()) {
		log::out << "Playground::moveObject - neither sever or client is runbing\n";
		return;
	}
	MessageBuffer msg;
	NetCommand cmd = MoveObject;
	msg.push(cmd);
	msg.push(&uuid);
	msg.push(&tr);

	if (m_server.isRunning()) {
		m_server.send(msg, 0, true);
	}
	if (m_client.isRunning()) {
		m_client.send(msg);
	}
}
void NetGameGround::netAddObject(engone::UUID uuid, int objectType, const std::string& modelAsset) {
	using namespace engone;
	if (!m_server.isRunning() && !m_client.isRunning()) {
		log::out << "Playground::moveObject - neither sever or client is runbing\n";
		return;
	}
	log::out << modelAsset<<"Add object\n";
	MessageBuffer msg;
	NetCommand cmd = AddObject;
	msg.push(cmd);
	msg.push(&uuid);
	msg.push(objectType);
	msg.push(modelAsset);

	if (m_server.isRunning()) {
		m_server.send(msg, 0, true);
	}
	if (m_client.isRunning()) {
		m_client.send(msg);
	}
}