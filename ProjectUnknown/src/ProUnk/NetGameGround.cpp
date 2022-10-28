#include "ProUnk/NetGameGround.h"
#include "Engone/Application.h"
#include "ProUnk/GameApp.h"

#include "ProUnk/Objects/Player.h"

namespace prounk {

	const char* to_string(NetCommand value) {
		switch (value) {
		case MoveObject: return "MoveObject";
		case AddObject: return "AddObject";
		case AnimateObject: return "AnimateObject";
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
			Sender* sender = m_server.isRunning() ? (Sender*)&m_server : (Sender*)&m_client;

			NetCommand cmd;
			msg.pull(&cmd);
			//log::out << m_server.isRunning() << " " << cmd << "\n";

			if (cmd == MoveObject) {
				//-- Extract data
				UUID uuid;
				msg.pull(&uuid);
				rp3d::Transform transform;
				rp3d::Vector3 velocity;
				rp3d::Vector3 angularVelocity;
				msg.pull(&transform);
				msg.pull(&velocity);
				msg.pull(&angularVelocity);

				//-- Find object with matching UUID
				GameObject* object = nullptr;
				for (int i = 0; i < m_objects.size(); i++) {
					if (m_objects[i]->getUUID() == uuid) {
						object = m_objects[i];
						break;
					}
				}

				if (object) {
					m_mutex.lock();
					object->rigidBody->setTransform(transform);
					object->rigidBody->setLinearVelocity(velocity);
					object->rigidBody->setAngularVelocity(angularVelocity);
					m_mutex.unlock();
				}

			} else if (cmd == AddObject) {
				//-- Extract data
				UUID uuid;
				msg.pull(&uuid); // ISSUE: what if uuid already exists (the client may have sent the same uuid by intention)
				int objectType;
				msg.pull(&objectType);
				std::string modelAsset;
				msg.pull(modelAsset);

				//if (objectType == 0) {
				GameObject* newObject = new GameObject(uuid);
				rp3d::Transform tr;
				tr.setPosition({ 0,0,0 });
				//newObject->setOnlyTrigger(true);
				// 
				newObject->rigidBody = m_pWorld->createRigidBody(tr);
				newObject->rigidBody->enableGravity(false);

				newObject->modelAsset = m_app->getStorage()->load<ModelAsset>(modelAsset, 0);
				newObject->loadColliders(this);
				addObject(newObject);
				if (objectType == 0)
					m_clients[uuid].player = newObject;
				//}
			} else if (cmd == AnimateObject) {
				//-- Extract data
				UUID uuid;
				msg.pull(&uuid);
				std::string instance;
				std::string animation;
				bool loop;
				float speed;
				float blend;
				float frame;

				msg.pull(&instance);
				msg.pull(&animation);
				msg.pull(&loop);
				msg.pull(&speed);
				msg.pull(&blend);
				msg.pull(&frame);

				//-- Find object with matching UUID
				GameObject* object = nullptr;
				for (int i = 0; i < m_objects.size(); i++) {
					if (m_objects[i]->getUUID() == uuid) {
						object = m_objects[i];
						break;
					}
				}

				if (object) {
					m_mutex.lock();
					object->animator.enable(instance, animation, { loop,speed,blend,frame });
					m_mutex.unlock();
				}

			} else if (cmd == EditObject) {
				//-- Extract data
				UUID uuid;
				msg.pull(&uuid);

				int type;
				uint64_t data;

				msg.pull(&type);
				msg.pull(&data);

				//-- Find object with matching UUID
				GameObject* object = nullptr;
				for (int i = 0; i < m_objects.size(); i++) {
					if (m_objects[i]->getUUID() == uuid) {
						object = m_objects[i];
						break;
					}
				}

				if (object) {
					m_mutex.lock();
					if (type == 0) {
						object->setOnlyTrigger(data != 0);
					} else if (type == 1) {
						object->rigidBody->setType((rp3d::BodyType)data);
						//UUID uuid2 = { data1,data2 };
						//GameObject* obj = getObject(uuid2);
						//if (obj) {
						//	Player* plr = (Player*)obj;
						//	object->rigidBody->getCollider(data0)->setUserData(&plr->combatData);
						//} else {
						//	object->rigidBody->getCollider(data0)->setUserData(nullptr);
						//}
					}
					m_mutex.unlock();
				}

			} else if (cmd == SyncObjects) {
				int count;
				msg.pull(&count);
				for (int j = 0; j < count; j++) {
					UUID uuid;
					msg.pull(&uuid);
					bool found = false;
					for (int i = 0; i < m_objects.size(); i++) {
						if (uuid == m_objects[i]->getUUID()) {
							found = true;
							break;
						}
					}
				}
			}
			return true; // wont work on server
		};
		m_server.setOnEvent([this](NetEvent e, UUID uuid) {
			log::out << "ProUnk::Server::onEvent - " << e << "\n";

			if (e == NetEvent::Connect) {
				m_clients[uuid] = {};
				prounk::GameApp* app = (prounk::GameApp*)m_app;
				for (int i = 0; i < m_objects.size(); i++) {
					GameObject* obj = m_objects[i];
					int type = 1;
					if (app->player == obj)
						type = 0;
					netAddObject(obj->getUUID(), type, obj->modelAsset->getLoadName());
					netEditObject(obj->getUUID(), 0, obj->isOnlyTrigger());
					netEditObject(obj->getUUID(), 1, (uint64_t)obj->rigidBody->getType());
					//netMoveObject(obj->getUUID(), obj->rigidBody->getTransform(),obj->rigidBody->getLinearVelocity(),obj->rigidBody->getAngularVelocity());
				}
			} else if (e == NetEvent::Disconnect) {
				m_clients.erase(uuid); // uuid should exist. NetworkModule may have a bug if it doesn't.
			}
			return true;
		});
		m_client.setOnEvent([this](NetEvent e, UUID uuid) {
			log::out << "ProUnk::Client::onEvent - " << e << "\n";

			if (e == NetEvent::Connect) {
				prounk::GameApp* app = (prounk::GameApp*)m_app;
				for (int i = 0; i < m_objects.size(); i++) {
					GameObject* obj = m_objects[i];
					int type = 1;
					if (app->player == obj)
						type = 0;
					netAddObject(obj->getUUID(), type, obj->modelAsset->getLoadName());
					//netMoveObject(obj->getUUID(), obj->rigidBody->getTransform(), obj->rigidBody->getLinearVelocity(), obj->rigidBody->getAngularVelocity());
				}
				//if (app->player) {
				//	netAddObject(app->player->getUUID(), 0, app->player->modelAsset->getLoadName());
				//}
			} else if (e == NetEvent::Disconnect) {
				// delete 
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
				if (m_clients.size() > 1) {
					MessageBuffer sendMsg = msg; // copy
					m_server.send(sendMsg, uuid, true); // relay received message to other clients
				}

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
	void NetGameGround::update(engone::UpdateInfo& info) {
		using namespace engone;
		GameGround::update(info);

		if (getServer().isRunning() || getClient().isRunning()) {
			for (int i = 0; i < m_objects.size(); i++) {
				GameObject* object = m_objects[i];
				if (object->flags & NetGameGround::OBJECT_NETMOVE) {
					netMoveObject(object->getUUID(), object->rigidBody->getTransform(),
						object->rigidBody->getLinearVelocity(), object->rigidBody->getAngularVelocity());
				}
			}
		}
	}
	//void NetGameGround::netSyncObjects() {
	//	using namespace engone;
	//	// When client connects, it requests uuids from server.
	//	// server sends them. ehm, this is complex. Simular complexity to UniSync where messages are sent back and forth.
	//	
	//	//-- Prepare message
	//	//MessageBuffer msg;
	//	//msg.push(SyncObjects);
	//	//int count = 0;
	//	//msg.push(count);
	//
	//	//-- send
	//}
	void NetGameGround::netMoveObject(engone::UUID uuid, const rp3d::Transform& transform, const rp3d::Vector3& velocity, const rp3d::Vector3& angular) {
		using namespace engone;
		if (!m_server.isRunning() && !m_client.isRunning()) {
			log::out << "Playground::moveObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = MoveObject;
		msg.push(cmd);
		msg.push(&uuid);
		msg.push(&transform);
		msg.push(&velocity);
		msg.push(&angular);

		//-- Send message
		if (m_server.isRunning())
			m_server.send(msg, 0, true);
		if (m_client.isRunning())
			m_client.send(msg);
	}
	void NetGameGround::netAddObject(engone::UUID uuid, engone::GameObject* object, int data) {
		using namespace engone;
		if (!m_server.isRunning() && !m_client.isRunning()) {
			log::out << log::RED << "Playground::moveObject - neither server or client is running\n";
			return;
		}
		log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = AddObject;
		msg.push(cmd);
		msg.push(&uuid);
		
		msg.push(objectType);
		msg.push(object->modelAsset->getLoadName());

		msg.push(data);

		//-- Send message
		if (m_server.isRunning())
			m_server.send(msg, 0, true);
		if (m_client.isRunning())
			m_client.send(msg);
	}
	void NetGameGround::netAnimateObject(engone::UUID uuid, const std::string& instance, const std::string& animation, bool loop, float speed, float blend, float frame) {
		using namespace engone;
		if (!m_server.isRunning() && !m_client.isRunning()) {
			log::out << "Playground::moveObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = MoveObject;
		msg.push(cmd);
		msg.push(&uuid);

		msg.push(&instance);
		msg.push(&animation);
		msg.push(&loop);
		msg.push(&speed);
		msg.push(&blend);
		msg.push(&frame);

		//-- Send message
		if (m_server.isRunning())
			m_server.send(msg, 0, true);
		if (m_client.isRunning())
			m_client.send(msg);
	}
	void NetGameGround::netEditObject(engone::UUID uuid, int type, uint64_t data) {
		using namespace engone;
		if (!m_server.isRunning() && !m_client.isRunning()) {
			log::out << "Playground::moveObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = EditObject;
		msg.push(cmd);
		msg.push(&uuid);

		msg.push(&type);
		msg.push(&data);

		//-- Send message
		if (m_server.isRunning())
			m_server.send(msg, 0, true);
		if (m_client.isRunning())
			m_client.send(msg);
	}
}