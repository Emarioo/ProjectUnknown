#include "ProUnk/World/Session.h"

#include "ProUnk/GameApp.h"

namespace prounk {
	const char* to_string(NetCommand value) {
		switch (value) {
		case MoveObject: return "MoveObject";
		case AddObject: return "AddObject";
		case DELETE_OBJECT: return "DeleteObject";
		case AnimateObject: return "AnimateObject";
		case EditObject: return "EditObject";
		case DamageObject: return "DamageObject";
		}
		return "Unknown";
	}
	engone::Logger& operator<<(engone::Logger& log, NetCommand value) {
		return log << to_string(value);
	}
	float NetworkStats::getBytesSentPerSecond() {
		return sentPerSecond;
	}
	float NetworkStats::getBytesReceivedPerSecond() {
		return receivedPerSecond;
	}
	void NetworkStats::updateSample(float delta, Session* session) {
		using namespace engone;
		uint32_t nowSent = session->getTotalBytesSent();
		uint32_t nowReceived = session->getTotalBytesReceived();
		sample_sent[(sample_index) % SAMPLE_COUNT] = nowSent - lastSent;
		sample_received[(sample_index) % SAMPLE_COUNT] = nowReceived - lastReceived;
		sample_time[(sample_index) % SAMPLE_COUNT] = delta;
		sample_index = (sample_index + 1) % SAMPLE_COUNT;
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

	Session::Session(GameApp* app) : m_parent(app) {
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
				std::string dimName;
				rp3d::Transform transform;
				rp3d::Vector3 velocity;
				rp3d::Vector3 angularVelocity;
				msg.pull(&uuid);
				msg.pull(dimName);
				msg.pull(&transform);
				msg.pull(&velocity);
				msg.pull(&angularVelocity);

				//-- Find object with matching UUID
				Dimension* dim = getDimension(dimName);
				if (dim) {
					EngineObject* object = dim->getWorld()->requestAccess(uuid);

					if (object) {
						//m_mutex.lock();
						object->getRigidBody()->setTransform(transform);
						object->getRigidBody()->setLinearVelocity(velocity);
						object->getRigidBody()->setAngularVelocity(angularVelocity);
						//m_mutex.unlock();
						dim->getWorld()->releaseAccess(uuid);
					}
				}

			} else if (cmd == AddObject) {
				//-- Extract data
				UUID uuid;
				std::string dimName;
				int objectType;
				std::string modelAsset;
				msg.pull(&uuid); // ISSUE: what if uuid already exists (the client may have sent the same uuid by intention)
				msg.pull(dimName);
				msg.pull(&objectType);
				msg.pull(modelAsset);

				//-- don't create object if uuid exists
				Dimension* dim = getDimension(dimName);
				if (dim) {
					
					EngineObject* object = dim->getWorld()->requestAccess(uuid);
					dim->getWorld()->releaseAccess(uuid);
					if (!object) {
						EngineObject* newObject = CreateObject(objectType, dim, uuid);
						newObject = dim->getWorld()->requestAccess(uuid);

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

						//addObject(newObject); //  CreateObject auto adds the object

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
						if(newObject)
							dim->getWorld()->releaseAccess(uuid);
					} else {
						log::out << log::RED << "World::Receive - Cannot add " << uuid << " because it exists\n";
					}
				}
				//}
			} else if (cmd == AnimateObject) {
				//-- Extract data
				UUID uuid;
				std::string dimName;
				std::string instance;
				std::string animation;
				bool loop;
				float speed;
				float blend;
				float frame;

				msg.pull(&uuid);
				msg.pull(&dimName);
				msg.pull(instance);
				msg.pull(animation);
				msg.pull(&loop);
				msg.pull(&speed);
				msg.pull(&blend);
				msg.pull(&frame);

				//-- Find object with matching UUID
				Dimension* dim = getDimension(dimName);
				if (dim) {
					EngineObject* object = dim->getWorld()->requestAccess(uuid);
						//dim->getWorld()->getObject(uuid);

					if (object) {
						if (object->getAnimator()) {
							//m_mutex.lock();
							object->getAnimator()->enable(instance, animation, { loop,speed,blend,frame });
							//m_mutex.unlock();
						}
					}
					dim->getWorld()->releaseAccess(uuid);
				}

			} else if (cmd == EditObject) {
				//-- Extract data
				UUID uuid;
				msg.pull(&uuid);

				std::string dimName;
				msg.pull(dimName);

				int type;
				msg.pull(&type);

				//-- Find object with matching UUID
				Dimension* dim = getDimension(dimName);
				if (dim) {
					EngineObject* object = dim->getWorld()->requestAccess(uuid);
					if (object) {
						//m_mutex.lock();
						if (type == 0) {
							uint64_t data;
							msg.pull(&data);
							object->setOnlyTrigger(data != 0);
						} else if (type == 1) {
							uint64_t data;
							msg.pull(&data);
							object->getRigidBody()->setType((rp3d::BodyType)data);
							//UUID uuid2 = { data1,data2 };
							//EngineObject* obj = getObject(uuid2);
							//if (obj) {
							//	Player* plr = (Player*)obj;
							//	object->rigidBody->getCollider(data0)->setUserData(&plr->combatData);
							//} else {
							//	object->rigidBody->getCollider(data0)->setUserData(nullptr);
							//}
						} else if (type == 2) {
							UUID uuid2;
							msg.pull(&uuid2);
							bool yes;
							msg.pull(&yes);

							EngineObject* object2 = dim->getWorld()->requestAccess(uuid2);
							if (object2) {
								log::out << log::RED << "Session::Receive - EditType:2 is broken\n";
								//SetCombatData(object2, object, yes);
							} else
								log::out << log::RED << "Session::Receive - " << cmd << ", type 2, object2 is nullptr\n";
							dim->getWorld()->releaseAccess(uuid2);
						}
						//m_mutex.unlock();
					}
					dim->getWorld()->releaseAccess(uuid);
				}

			} else if (cmd == DamageObject) {
				//-- Extract data
				UUID uuid;
				msg.pull(&uuid);

				std::string dimName;
				msg.pull(dimName);

				float damage;
				msg.pull(&damage);

				//-- Find object with matching UUID
				Dimension* dim = getDimension(dimName);
				if (dim) {
					//EngineObject* object = dim->getWorld()->requestAccess(uuid);
					//EngineObject* object = dim->getWorld()->getObject(uuid);

					log::out << "Session::Receive : DamageObject is broken\n";
					//if (object) {
					//	//m_mutex.lock();
					//	CombatData* combatData = entityRegistry.getEntry(object->userData).combatData;
					//	combatData->health -= damage;
					//	if (combatData->health < 0) combatData->health = 0;

					//	// temporary. a more sophisticated particle request system will be made
					//	this->getApp()->doParticles(object->getPosition());

					//	//m_mutex.unlock();
					//}
				}
			} else if (cmd == DELETE_OBJECT) {
				UUID uuid;
				msg.pull(&uuid);

				std::string dimName;
				msg.pull(dimName);
				Dimension* dim = getDimension(dimName);
				if (dim) {
					EngineObject* object = dim->getWorld()->requestAccess(uuid);
					//m_mutex.lock();
					// this is highly unoptimized
					int index = -1;

					//dim->getWorld()->deleteObject(object);
					if (object) {
						DeleteObject(dim, object);
						dim->getWorld()->releaseAccess(uuid);
					} else {
						log::out << "Session::Receive - Invalid object to delete\n";
					}
					//for (int j = 0; j < m_objects.size(); j++) {
					//	if (uuid == m_objects[j]->getUUID()) { // NOTE: compare UUID instead of pointers?
					//		index = j;
					//	}
					//}
					//if (index != -1) {
					//	DeleteObject(this, m_objects[index]);
					//	m_objects.erase(m_objects.begin() + index);
					//} else {
					//	log::out << log::RED << "World::Event - Object " << uuid << " is already deleted\n";
					//}
					//m_mutex.unlock();
				}
			}
			return true; // wont work on server
		};
		m_server.setOnEvent([this](NetEvent e, UUID uuid) {
			log::out << "ProUnk::Server::onEvent - " << e << "\n";

			if (e == NetEvent::Connect) {
				m_clients[uuid] = {};
				prounk::GameApp* app = m_parent;
				Dimension* dim = getDimension("0");
				EngineObjectIterator iter = dim->getWorld()->createIterator();
				EngineObject* obj;
				while (obj = iter.next()) {
					netAddObject(obj->getUUID(), obj);
					netEditObject(obj->getUUID(), 0, obj->isOnlyTrigger());
					netEditObject(obj->getUUID(), 1, (uint64_t)obj->getRigidBody()->getType());
					//netMoveObject(obj->getUUID(), obj->rigidBody->getTransform(),obj->rigidBody->getLinearVelocity(),obj->rigidBody->getAngularVelocity());
				}
				//dim->getWorld()->deleteIterator(iter);
			} else if (e == NetEvent::Disconnect) {
				auto find = m_clients.find(uuid);
				if (find == m_clients.end()) {
					// this would be a bug
					log::out << log::RED << "World::Event - Cannot find client data for " << uuid << "\n";
				} else {
					//m_mutex.lock();
					Dimension* dim = getDimension("0");
					for (int i = 0; i < find->second.ownedObjects.size(); i++) {
						EngineObject* obj = find->second.ownedObjects[i];
						// this is highly unoptimized

						EngineObjectIterator iter = dim->getWorld()->createIterator();
						EngineObject* obj2;
						bool found = false;
						while (obj2 = iter.next()) {
							if (obj == obj2) { // NOTE: compare UUID instead of pointers?
								found = true;
								break;
							}
						}
						//dim->getWorld()->deleteIterator(iter);
						if (found) {
							netDeleteObject(obj->getUUID());
							DeleteObject(dim, find->second.ownedObjects[i]);
						} else {
							log::out << log::RED << "Session::Event - Object is not owned?\n";

						}
					}
					//m_mutex.unlock();
					m_clients.erase(find);
				}
			}
			return true;
		});
		m_client.setOnEvent([this](NetEvent e, UUID uuid) {
			log::out << "ProUnk::Client::onEvent - " << e << "\n";

			if (e == NetEvent::Connect) {
				prounk::GameApp* app = (prounk::GameApp*)m_parent;

				Dimension* dim = getDimension("0");
				EngineObjectIterator iter = dim->getWorld()->createIterator();
				EngineObject* obj;
				while (obj = iter.next()) {
					if (obj->getFlags() & OBJECT_NETMOVE)
						netAddObject(obj->getUUID(), obj);
					//netMoveObject(obj->getUUID(), obj->rigidBody->getTransform(), obj->rigidBody->getLinearVelocity(), obj->rigidBody->getAngularVelocity());
				}
				//for (int i = 0; i < m_objects.size(); i++) {
				//	EngineObject* obj = m_objects[i];
				//	if (obj->flags & OBJECT_NETMOVE)
				//		netAddObject(obj->getUUID(), obj);
					//netMoveObject(obj->getUUID(), obj->rigidBody->getTransform(), obj->rigidBody->getLinearVelocity(), obj->rigidBody->getAngularVelocity());
				//}
				//if (app->player) {
				//	netAddObject(app->player->getUUID(), 0, app->player->modelAsset->getLoadName());
				//}
			} else if (e == NetEvent::Disconnect) {
				// delete all objects
				//m_mutex.lock();
				Dimension* dim = getDimension("0");
				EngineObjectIterator iter = dim->getWorld()->createIterator();
				EngineObject* obj;
				while (obj = iter.next()) {
					if ((obj->getFlags() & OBJECT_NETMOVE) == 0) {
						DeleteObject(dim, obj);
					}
				}
				//m_mutex.unlock();
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
	Session::~Session() {
		cleanup();
	}
	void Session::cleanup() {
		using namespace engone;
		log::out << log::RED << "Session : CLEANUP IS NOT IMPLEMENTED!\n";
	}

	std::vector<Dimension*>& Session::getDimensions() {
		return m_dimensions;
	}
	Dimension* Session::createDimension(const std::string& name) {
		Dimension* dim = getDimension(name);
		if (dim)
			return nullptr; // name is not unique
		
		dim = ALLOC_NEW(Dimension)(this);
		dim->name = name;
		m_dimensions.push_back(dim);
		return dim;
	}
	Dimension* Session::getDimension(const std::string& name) {
		for (int i = 0; i < m_dimensions.size(); i++) {
			if (m_dimensions[i]->name == name) {
				return m_dimensions[i];
			}
		}
		return nullptr;
	}
	GameApp* Session::getParent() {
		return m_parent;
	}
	void Session::update(engone::LoopInfo& info) {
		using namespace engone;

		for (int i = 0; i < m_dimensions.size(); i++) {
			m_dimensions[i]->update(info);
		}

		m_networkStats.updateSample(info.timeStep,this);
	}
	engone::Client& Session::getClient() {
		return m_client;
	}
	engone::Server& Session::getServer() {
		return m_server;
	}
	NetworkStats& Session::getNetworkStats() {
		return m_networkStats;
	}
	uint32_t Session::getTotalBytesSent() {
		if (m_server.isRunning()) {
			return m_server.getStats().sentBytes();
		}
		if (m_client.isRunning()) {
			return m_client.getStats().sentBytes();
		}
		return 0;
	}
	uint32_t Session::getTotalBytesReceived() {
		if (m_server.isRunning()) {
			return m_server.getStats().receivedBytes();
		}
		if (m_client.isRunning()) {
			return m_client.getStats().receivedBytes();
		}
		return 0;
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
		
		Dimension* dim = getSession()->getDimension("0");
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = MoveObject;
		msg.push(cmd);
		// creating the values on stack here is unnecessary but requires
		// changes to MessageBuffer to be able to avoid it.
		UUID uuid = object->getUUID();
		rp3d::Vector3 vel = object->getRigidBody()->getLinearVelocity();
		rp3d::Vector3 angVel = object->getRigidBody()->getAngularVelocity();
		msg.push(&uuid);
		msg.push(dim->getName());
		msg.push(&object->getRigidBody()->getTransform());
		msg.push(&vel);
		msg.push(&angVel);

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netAddObject(engone::UUID uuid, engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		//log::out << m_server.isRunning() << " " << object->modelAsset->getLoadName() << " Add object\n";

		Dimension* dim = getSession()->getDimension("0");

		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = AddObject;
		msg.push(cmd);
		msg.push(&uuid);
		msg.push(dim->getName());

		int type = object->getObjectType();
		msg.push(&type);
		msg.push(object->getModel()->getLoadName());

		//-- Send message
		networkSend(msg);
	}
	void  NetworkFuncs::netDeleteObject(engone::UUID uuid) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << log::RED << "World::addObject - neither server or client is running\n";
			return;
		}
		Dimension* dim = getSession()->getDimension("0");
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = DELETE_OBJECT;
		msg.push(cmd);
		msg.push(&uuid);
		msg.push(dim->getName());
		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netAnimateObject(engone::UUID uuid, const std::string& instance, const std::string& animation, bool loop, float speed, float blend, float frame) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::netAnimateObject - neither server or client is running\n";
			return;
		}
		Dimension* dim = getSession()->getDimension("0");
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = AnimateObject;
		msg.push(cmd);
		msg.push(&uuid);
		msg.push(dim->getName());

		msg.push(instance);
		msg.push(animation);
		msg.push(&loop);
		msg.push(&speed);
		msg.push(&blend);
		msg.push(&frame);

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netEditObject(engone::UUID uuid, int type, uint64_t data) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::editObject - neither server or client is running\n";
			return;
		}
		Dimension* dim = getSession()->getDimension("0");
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = EditObject;
		msg.push(cmd);
		msg.push(&uuid);
		msg.push(dim->getName());

		msg.push(&type);

		msg.push(&data);

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netEditCombatData(engone::UUID uuid, engone::UUID player, bool yes) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "World::editCombatObject - neither server or client is running\n";
			return;
		}
		Dimension* dim = getSession()->getDimension("0");
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = EditObject;
		msg.push(cmd);
		msg.push(&uuid);
		msg.push(dim->getName());
		int type = 2;
		msg.push(&type);

		msg.push(&player);
		msg.push(&yes);

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netDamageObject(engone::UUID uuid, float damage) {
		using namespace engone;
		if (!networkRunning()) {
			//log::out << "Playground::moveObject - neither server or client is running\n";
			return;
		}
		Dimension* dim = getSession()->getDimension("0");
		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = DamageObject;
		msg.push(cmd);
		msg.push(&uuid);
		msg.push(dim->getName());
		msg.push(&damage);

		//-- Send message
		networkSend(msg);
	}
	bool NetworkFuncs::networkRunning() {
		return getClient().isRunning() || getServer().isRunning();
	}
	void NetworkFuncs::networkSend(engone::MessageBuffer& msg) {
		if (getServer().isRunning())
			getServer().send(msg, 0, true);
		if (getClient().isRunning())
			getClient().send(msg);
	}
}