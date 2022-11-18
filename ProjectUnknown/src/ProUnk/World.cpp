#include "ProUnk/World.h"
#include "Engone/Application.h"
#include "ProUnk/GameApp.h"

#include "ProUnk/Combat/Tags.h"

#include "ProUnk/Combat/Combat.h"

#include "ProUnk/Objects/BasicObjects.h"

namespace prounk {

	const char* to_string(NetCommand value) {
		switch (value) {
		case MoveObject: return "MoveObject";
		case AddObject: return "AddObject";
		case DELETE_OBJECT: return "DeleteObject";
		case AnimateObject: return "AnimateObject";
		case EditObject: return "EditObject";
		case DamageObject: return "DamageObject";
		case SyncObjects: return "SyncObjects";
		}
		return "Unknown";
	}
	engone::Logger& operator<<(engone::Logger& log, NetCommand value) {
		return log << to_string(value);
	}
	World::World(engone::Application* app) {
		m_app = app;
		using namespace engone;
		auto onRecv = [this](MessageBuffer& msg, UUID clientUUID) {
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
				EngineObject* object = getObject(uuid);

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

				//-- don't create object if uuid exists
				EngineObject* object = getObject(uuid);
				if (!object) {
					EngineObject* newObject = CreateObject(objectType, this, uuid);

					//if (objectType == 0) {
					//EngineObject* newObject = new EngineObject(uuid);
					//rp3d::Transform tr;
					//tr.setPosition({ 0,0,0 });
					//newObject->setOnlyTrigger(true);
					//// 
					//newObject->rigidBody = m_pWorld->createRigidBody(tr);
					//newObject->rigidBody->enableGravity(false);
					//newObject->rigidBody->getUs
					//newObject->modelAsset = m_app->getStorage()->load<ModelAsset>(modelAsset, 0);
					//newObject->animator.asset = newObject->modelAsset;
					//newObject->loadColliders(this);
					addObject(newObject);

					if (clientUUID != 0) {
						auto find = m_clients.find(clientUUID);
						if (find == m_clients.end()) {
							log::out << log::RED << "World::Recieve - Could not find client data for " << clientUUID << "\n";
						} else {
							if (objectType == OBJECT_PLAYER)
								find->second.player = newObject;

							find->second.ownedObjects.push_back(newObject);
						}
					}
				}
				else {
					log::out << log::RED << "World::Receive - Cannot add "<<uuid<<" because it exists\n";
				}
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

				msg.pull(instance);
				msg.pull(animation);
				msg.pull(&loop);
				msg.pull(&speed);
				msg.pull(&blend);
				msg.pull(&frame);

				//-- Find object with matching UUID
				EngineObject* object = getObject(uuid);

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
				msg.pull(&type);

				//-- Find object with matching UUID
				EngineObject* object = getObject(uuid);

				if (object) {
					m_mutex.lock();
					if (type == 0) {
						uint64_t data;
						msg.pull(&data);
						object->setOnlyTrigger(data != 0);
					} else if (type == 1) {
						uint64_t data;
						msg.pull(&data);
						object->rigidBody->setType((rp3d::BodyType)data);
						//UUID uuid2 = { data1,data2 };
						//EngineObject* obj = getObject(uuid2);
						//if (obj) {
						//	Player* plr = (Player*)obj;
						//	object->rigidBody->getCollider(data0)->setUserData(&plr->combatData);
						//} else {
						//	object->rigidBody->getCollider(data0)->setUserData(nullptr);
						//}
					}
					else if (type == 2) {
						UUID uuid2;
						msg.pull(&uuid2);
						bool yes;
						msg.pull(&yes);

						EngineObject* object2 = getObject(uuid2);
						if (object2)
							SetCombatData(object2, object, yes);
						else
							log::out << log::RED << "World::Receive - "<<cmd<<", type 2, object2 is nullptr\n";
					}
					m_mutex.unlock();
				}

			}
			else if (cmd == DamageObject) {
				//-- Extract data
				UUID uuid;
				msg.pull(&uuid);

				float damage;
				msg.pull(&damage);

				//-- Find object with matching UUID
				EngineObject* object = getObject(uuid);

				if (object) {
					m_mutex.lock();
					CombatData* combatData = entityHandler.getEntry(object->userData).combatData;
					combatData->health -= damage;
					if(combatData->health<0) combatData->health = 0;

					// temporary. a more sophisticated particle request system will be made
					this->getApp()->doParticles(object->getPosition());

					m_mutex.unlock();
				}
			} else if (cmd == DELETE_OBJECT) {
				UUID uuid;
				msg.pull(&uuid);

				m_mutex.lock();
				// this is highly unoptimized
				int index = -1;
				
				for (int j = 0; j < m_objects.size(); j++) {
					if (uuid == m_objects[j]->getUUID()) { // NOTE: compare UUID instead of pointers?
						index = j;
					}
				}
				if (index != -1) {
					DeleteObject(this, m_objects[index]);
					m_objects.erase(m_objects.begin() + index);
				} else {
					log::out << log::RED << "World::Event - Object " << uuid << " is already deleted\n";
				}
				m_mutex.unlock();
			} else if (cmd == SyncObjects) {
				//int count;
				//msg.pull(&count);
				//for (int j = 0; j < count; j++) {
				//	UUID uuid;
				//	msg.pull(&uuid);
				//	bool found = false;
				//	for (int i = 0; i < m_objects.size(); i++) {
				//		if (uuid == m_objects[i]->getUUID()) {
				//			found = true;
				//			break;
				//		}
				//	}
				//}
			}
			return true; // wont work on server
		};
		m_server.setOnEvent([this](NetEvent e, UUID uuid) {
			log::out << "ProUnk::Server::onEvent - " << e << "\n";

			if (e == NetEvent::Connect) {
				m_clients[uuid] = {};
				prounk::GameApp* app = (prounk::GameApp*)m_app;
				for (int i = 0; i < m_objects.size(); i++) {
					EngineObject* obj = m_objects[i];
					
					netAddObject(obj->getUUID(), obj);
					netEditObject(obj->getUUID(), 0, obj->isOnlyTrigger());
					netEditObject(obj->getUUID(), 1, (uint64_t)obj->rigidBody->getType());
					//netMoveObject(obj->getUUID(), obj->rigidBody->getTransform(),obj->rigidBody->getLinearVelocity(),obj->rigidBody->getAngularVelocity());
				}
			} else if (e == NetEvent::Disconnect) {
				auto find = m_clients.find(uuid);
				if (find==m_clients.end()) {
					// this would be a bug
					log::out << log::RED << "World::Event - Cannot find client data for "<<uuid<<"\n";
				} else {
					m_mutex.lock();
					for (int i = 0; i < find->second.ownedObjects.size();i++) {
						EngineObject* obj = find->second.ownedObjects[i];
						// this is highly unoptimized
						int index = -1;
						for (int j = 0; j < m_objects.size(); j++) {
							if (find->second.ownedObjects[i] == m_objects[j]) { // NOTE: compare UUID instead of pointers?
								index = j;
							}
						}
						netDeleteObject(obj->getUUID());
						if (index != -1) {
							DeleteObject(this, find->second.ownedObjects[i]);
							m_objects.erase(m_objects.begin() + index);
						} else {
							log::out << log::RED << "World::Event - Object "<<uuid<<" is already deleted\n";
						}

					}
					m_mutex.unlock();
					m_clients.erase(find);
				}
			}
			return true;
		});
		m_client.setOnEvent([this](NetEvent e, UUID uuid) {
			log::out << "ProUnk::Client::onEvent - " << e << "\n";

			if (e == NetEvent::Connect) {
				prounk::GameApp* app = (prounk::GameApp*)m_app;
				for (int i = 0; i < m_objects.size(); i++) {
					EngineObject* obj = m_objects[i];
					if(obj->flags & OBJECT_NETMOVE)
						netAddObject(obj->getUUID(), obj);
					//netMoveObject(obj->getUUID(), obj->rigidBody->getTransform(), obj->rigidBody->getLinearVelocity(), obj->rigidBody->getAngularVelocity());
				}
				//if (app->player) {
				//	netAddObject(app->player->getUUID(), 0, app->player->modelAsset->getLoadName());
				//}
			} else if (e == NetEvent::Disconnect) {
				// delete all objects
				m_mutex.lock();
				for (int i = 0; i < m_objects.size(); i++) {
					EngineObject* obj = m_objects[i];
					if ((obj->flags & OBJECT_NETMOVE) == 0) {
						DeleteObject(this, obj);
						m_objects.erase(m_objects.begin() + i);
						i--;
					}
				}
				m_mutex.unlock();
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
	void World::cleanup() {
		World::cleanup();
	}
	void World::update(engone::LoopInfo& info) {
		using namespace engone;
		EngineWorld::update(info); // lock this with mutex? maybe not?

		// dummy AI (not efficient)
		
		EngineObjectIterator iterator = getIterator();
		EngineObject* obj;
		while (obj = iterator.next()){
			if (obj->objectType==OBJECT_DUMMY) {
				CombatData* combatData = getCombatData(obj);

				if (combatData->health == 0) {
					// delete or respawn dummy
					DeleteObject(this, obj);
					iterator.popCurrent();
					continue;
				}

				//if (GetRandom() < 0.02f) {
					combatData->animationTime = 1.f; // required otherwise attacking will be set to false
					combatData->attack(); // attacks every tick.
					//log::out << "hit\n";
				//}
				float minDist = 9999999;
				EngineObject* plr = nullptr;
				EngineObjectIterator iterator2 = getIterator();
				EngineObject* obj2;
				while (obj2 = iterator2.next()) {
				//for (int j = 0; j < m_objects.size(); j++) {
					//GameObject* obj2 = m_objects[j];
					if (obj2->objectType == OBJECT_PLAYER) {
						if ((obj2->flags & OBJECT_IS_DEAD)==0) {
							float leng = glm::length(obj->getPosition() - obj2->getPosition());
							if (leng < minDist || !plr) {
								plr = obj2;
								minDist = leng;
							}
						}
					}
				}
				if (plr) {
					glm::vec3 diff = plr->getPosition() - obj->getPosition();
					float speed = 0.17f;
					if (glm::length(diff) > 10) {
						speed *= 2;
					}
					glm::vec3 wantedVelocity = speed * glm::normalize(diff)/info.timeStep;

					glm::vec3 currentVelocity = ToGlmVec3(obj->rigidBody->getLinearVelocity());
					glm::vec3 force = 0.045f*(wantedVelocity - 0.7f*currentVelocity) / info.timeStep;
					//glm::vec3 force = 0.01f*(wantedVelocity - 0.4f*currentVelocity) / info.timeStep;
					
					obj->rigidBody->applyWorldForceAtCenterOfMass(ToRp3dVec3(force));
				}
			}
		}

		if (getServer().isRunning() || getClient().isRunning()) {
			EngineObjectIterator iterator = getIterator();
			//EngineObject* obj;
			while (obj = iterator.next()) {
			//for (int i = 0; i < m_objects.size(); i++) {
				//GameObject* object = m_objects[i];
				if (obj->flags & World::OBJECT_NETMOVE) {
					netMoveObject(obj);
					//netMoveObject(object->getUUID(), object->rigidBody->getTransform(),
					//	object->rigidBody->getLinearVelocity(), object->rigidBody->getAngularVelocity());
				}
			}
		}
		// combatdata
		iterator = getIterator();
		//EngineObject* obj;
		while (obj = iterator.next()) {
		//for (int i = 0; i < m_objects.size(); i++) {
			//GameObject* obj = m_objects[i];
			if (obj->flags & OBJECT_HAS_COMBATDATA) {
				CombatData* combatData = entityHandler.getEntry(obj->userData).combatData;

				// wasUpdate is temporary. objects may share combatData and it may therefore be updated multiple times.
				// this prevents that. A permanant solution would be to have combatData in an contigouous list
				// which you can loop through. CombatData will not be updated twice and it is better for cache.
				if (!combatData->wasUpdated) {
					combatData->update(info);
					combatData->wasUpdated = true;
				}
			}
		}
		iterator = getIterator();
		//EngineObject* obj;
		while (obj = iterator.next()) {
		//for (int i = 0; i < m_objects.size(); i++) {
			//GameObject* obj = m_objects[i];
			if (obj->flags & OBJECT_HAS_COMBATDATA) {
				CombatData* combatData = entityHandler.getEntry(obj->userData).combatData;
				combatData->wasUpdated=false;
			}
		}
		//m_objectMutex.unlock();
		updateSample(info.timeStep);
	}
	//void World::netSyncObjects() {
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

	void World::netMoveObject(engone::EngineObject* object) {
		using namespace engone;
		if (!m_server.isRunning() && !m_client.isRunning()) {
			//log::out << "World::netMoveObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = MoveObject;
		msg.push(cmd);
		// creating the values on stack here is unnecessary but requires
		// changes to MessageBuffer to be able to avoid it.
		UUID uuid = object->getUUID();
		rp3d::Vector3 vel = object->rigidBody->getLinearVelocity();
		rp3d::Vector3 angVel = object->rigidBody->getAngularVelocity();
		msg.push(&uuid);
		msg.push(&object->rigidBody->getTransform());
		msg.push(&vel);
		msg.push(&angVel);

		//-- Send message
		if (m_server.isRunning())
			m_server.send(msg, 0, true);
		if (m_client.isRunning())
			m_client.send(msg);
	}
	//void World::netMoveObject(engone::UUID uuid, const rp3d::Transform& transform, const rp3d::Vector3& velocity, const rp3d::Vector3& angular) {
	//	using namespace engone;
	//	if (!m_server.isRunning() && !m_client.isRunning()) {
	//		//log::out << "World::netMoveObject - neither server or client is running\n";
	//		return;
	//	}
	//	//-- Prepare message
	//	MessageBuffer msg;
	//	NetCommand cmd = MoveObject;
	//	msg.push(cmd);
	//	msg.push(&uuid);
	//	msg.push(&transform);
	//	msg.push(&velocity);
	//	msg.push(&angular);

	//	//-- Send message
	//	if (m_server.isRunning())
	//		m_server.send(msg, 0, true);
	//	if (m_client.isRunning())
	//		m_client.send(msg);
	//}
	void World::netAddObject(engone::UUID uuid, engone::EngineObject* object) {
		using namespace engone;
		if (!m_server.isRunning() && !m_client.isRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = AddObject;
		msg.push(cmd);
		msg.push(&uuid);
		
		msg.push(&object->objectType);
		msg.push(object->modelAsset->getLoadName());

		//-- Send message
		if (m_server.isRunning())
			m_server.send(msg, 0, true);
		if (m_client.isRunning())
			m_client.send(msg);
	}
	void  World::netDeleteObject(engone::UUID uuid) {
		using namespace engone;
		if (!m_server.isRunning() && !m_client.isRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = DELETE_OBJECT;
		msg.push(cmd);
		msg.push(&uuid);

		//-- Send message
		if (m_server.isRunning())
			m_server.send(msg, 0, true);
		if (m_client.isRunning())
			m_client.send(msg);
	}
	void World::netAnimateObject(engone::UUID uuid, const std::string& instance, const std::string& animation, bool loop, float speed, float blend, float frame) {
		using namespace engone;
		if (!m_server.isRunning() && !m_client.isRunning()) {
			//log::out << "World::netAnimateObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = AnimateObject;
		msg.push(cmd);
		msg.push(&uuid);

		msg.push(instance);
		msg.push(animation);
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
	void World::netEditObject(engone::UUID uuid, int type, uint64_t data) {
		using namespace engone;
		if (!m_server.isRunning() && !m_client.isRunning()) {
			//log::out << "World::editObject - neither server or client is running\n";
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
	void World::netEditCombatData(engone::UUID uuid, engone::UUID player, bool yes) {
		using namespace engone;
		if (!m_server.isRunning() && !m_client.isRunning()) {
			//log::out << "World::editCombatObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = EditObject;
		msg.push(cmd);
		msg.push(&uuid);
		int type = 2;
		msg.push(&type);

		msg.push(&player);
		msg.push(&yes);

		//-- Send message
		if (m_server.isRunning())
			m_server.send(msg, 0, true);
		if (m_client.isRunning())
			m_client.send(msg);
	}
	void World::netDamageObject(engone::UUID uuid, float damage) {
		using namespace engone;
		if (!m_server.isRunning() && !m_client.isRunning()) {
			//log::out << "Playground::moveObject - neither server or client is running\n";
			return;
		}
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = DamageObject;
		msg.push(cmd);
		msg.push(&uuid);

		msg.push(&damage);

		//-- Send message
		if (m_server.isRunning())
			m_server.send(msg, 0, true);
		if (m_client.isRunning())
			m_client.send(msg);
	}
	uint32_t World::getTotalBytesSent() {
		if (m_server.isRunning()) {
			return m_server.getStats().sentBytes();
		}
		if (m_client.isRunning()) {
			return m_client.getStats().sentBytes();
		}
		return 0;
	}
	uint32_t World::getTotalBytesReceived() {
		if (m_server.isRunning()) {
			return m_server.getStats().receivedBytes();
		}
		if (m_client.isRunning()) {
			return m_client.getStats().receivedBytes();
		}
		return 0;
	}
	float World::getBytesSentPerSecond() {
		return sentPerSecond;
	}
	float World::getBytesReceivedPerSecond() {
		return receivedPerSecond;
	}
	void World::updateSample(float delta) {
		using namespace engone;
		uint32_t nowSent = getTotalBytesSent();
		uint32_t nowReceived = getTotalBytesReceived();
		sample_sent[(sample_index) % SAMPLE_COUNT] = nowSent - lastSent;
		sample_received[(sample_index) % SAMPLE_COUNT] = nowReceived - lastReceived;
		sample_time[(sample_index) % SAMPLE_COUNT] = delta;
		sample_index=(sample_index+1)%SAMPLE_COUNT;
		lastSent = nowSent;
		lastReceived = nowReceived;

		uint32_t totalSampleSent = 0;
		uint32_t totalSampleReceived = 0;
		float time = 0;
		for (int i = 0; i < SAMPLE_COUNT; i++) {
			totalSampleSent += sample_sent[i];
			totalSampleReceived += sample_received[i];
			time += sample_time[i];
		}
		// the reason sent on client is different from receiving on server is that the messages aren't gurranteed to be received before update function.
		// the receiving is asynchounous and running on a seperate thread.
		sentPerSecond = totalSampleSent / time;
		receivedPerSecond = totalSampleReceived / time;
		//printf("? %u %u\n", nowSent,nowReceived);
		//Sender* eh = &m_client;
		//if (m_server.isRunning())
		//	eh = &m_server;
		//printf("? %u %u\n", eh->getStats().sentMessages(), eh->getStats().receivedMessages());
	}
	CombatData* World::getCombatData(engone::EngineObject* object) {
		if (object->userData != 0) {
			return entityHandler.getEntry(object->userData).combatData;
		}
		return nullptr;
	}
	Inventory* World::getInventory(engone::EngineObject* object) {
		if (object->userData != 0) {
			int inv = entityHandler.getEntry(object->userData).inventoryIndex;
			if (inv != 0)
				return inventoryHandler.getInventory(object->userData);
		}
		return nullptr;
	}
}