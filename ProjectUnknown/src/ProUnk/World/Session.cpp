#include "ProUnk/World/Session.h"

#include "ProUnk/GameApp.h"

namespace prounk {
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
#ifdef gone
		auto onRecv = [this](MessageBuffer& msg, UUID clientUUID) {
			//log::out << "Receive: Size:" <<msg.size()<<" UUID: "<<uuid<<"\n";
			Sender* sender = m_server.isRunning() ? (Sender*)&m_server : (Sender*)&m_client;

			NetCommand cmd;
			msg.pull(&cmd);
			//log::out << m_server.isRunning() << " " << cmd << "\n";

			if (cmd == MoveObject) {
				//-- Extract data
				rp3d::Transform transform;
				rp3d::Vector3 velocity;
				rp3d::Vector3 angularVelocity;
				UUID_DIM obj;
				pullObject(msg, obj);
				msg.pull(&transform);
				msg.pull(&velocity);
				msg.pull(&angularVelocity);

				//-- Find object with matching UUID
				if (obj.dim) {
					EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);

					if (object) {
						object->getRigidBody()->setTransform(transform);
						object->getRigidBody()->setLinearVelocity(velocity);
						object->getRigidBody()->setAngularVelocity(angularVelocity);
						obj.dim->getWorld()->releaseAccess(obj.uuid);
					}
				}

			} else if (cmd == NET_ADD_TERRAIN) {
				recAddTerrain(msg, clientUUID);
			} else if (cmd == NET_ADD_ITEM) {
				recAddItem(msg, clientUUID);
			} else if (cmd == NET_ADD_WEAPON) {
				recAddWeapon(msg, clientUUID);
			} else if (cmd == NET_ADD_CREATURE) {
				recAddCreature(msg, clientUUID);
			} else if (cmd == AnimateObject) {
				//-- Extract data
				UUID_DIM obj;
				pullObject(msg, obj);

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
				if (obj.dim) {
					EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);
						//dim->getWorld()->getObject(uuid);

					if (object) {
						if (object->getAnimator()) {
							//m_mutex.lock();
							object->getAnimator()->enable(instance, animation, { loop,speed,blend,frame });
							//m_mutex.unlock();
						}
						obj.dim->getWorld()->releaseAccess(obj.uuid);
					}
				}

			} else if (cmd == EditObject) {
				//-- Extract data
				UUID_DIM obj;
				pullObject(msg, obj);

				int type;
				msg.pull(&type);

				//-- Find object with matching UUID
				if (obj.dim) {
					EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);
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

							EngineObject* object2 = obj.dim->getWorld()->requestAccess(uuid2);
							if (object2) {
								log::out << log::RED << "Session::Receive - EditType:2 is broken\n";
								//SetCombatData(object2, object, yes);
								obj.dim->getWorld()->releaseAccess(uuid2);
							} else
								log::out << log::RED << "Session::Receive - " << cmd << ", type 2, object2 is nullptr\n";
						}
						obj.dim->getWorld()->releaseAccess(obj.uuid);
					}
				}

			} else if (cmd == DamageObject) {
				//-- Extract data
				UUID_DIM obj;
				pullObject(msg, obj);

				float damage;
				msg.pull(&damage);

				//-- Find object with matching UUID
				if (obj.dim) {
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
				UUID_DIM obj;
				pullObject(msg, obj);
				if (obj.dim) {
					EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);
					// this is highly unoptimized

					if (object) {
						DeleteObject(obj.dim, object);
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
					netAddGeneral(obj);
					netEditObject(obj, 0, obj->isOnlyTrigger());
					netEditObject(obj, 1, (uint64_t)obj->getRigidBody()->getType());
					//netMoveObject(obj->getUUID(), obj->rigidBody->getTransform(),obj->rigidBody->getLinearVelocity(),obj->rigidBody->getAngularVelocity());
				}
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
							netDeleteObject(obj);
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
						netAddGeneral(obj);
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
			} else {
				// no data found for client. This should not happen because data was made when client connected.
				log::out << log::RED << "Session - no data for client!\n";
			}
			return true;
		});
		m_client.setOnReceive(onRecv);
#endif
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
}