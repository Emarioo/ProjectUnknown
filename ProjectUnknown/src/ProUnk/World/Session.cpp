#include "ProUnk/World/Session.h"

#include "ProUnk/GameApp.h"

namespace prounk {
	const char* Session::DEFAULT_PORT = "3567";

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
		m_server.setOnEvent([this](NetEvent e, UUID uuid) {
			log::out << "Server : " << e << "\n";

			m_messageQueueMutex.lock();
			m_messageQueue.push_back({ true, e,uuid });
			m_messageQueueMutex.unlock();
			
			return true;
		});
		m_client.setOnEvent([this](NetEvent e, UUID uuid) {
			log::out << "Client : " << e << "\n";

			m_messageQueueMutex.lock();
			m_messageQueue.push_back({ false, e, uuid });
			m_messageQueueMutex.unlock();

			return true;
		});
		m_server.setOnReceive([this](MessageBuffer& msg, UUID uuid) {
			//log::out << "Relay ";

			 //detect command.
			//NetMessageType cmd;
			//msg.pull(&cmd);

			// check if you have permission.
			auto find = m_clients.find(uuid);
			if (find != m_clients.end()) {

				MessageBuffer* copy = msg.copy();

				m_messageQueueMutex.lock();
				m_messageQueue.push_back({ copy,uuid });
				m_messageQueueMutex.unlock();

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
		m_client.setOnReceive([this](MessageBuffer& msg, UUID uuid) {

			MessageBuffer* copy = msg.copy();

			m_messageQueueMutex.lock();
			m_messageQueue.push_back({ copy,uuid });
			m_messageQueueMutex.unlock();

			return true;
		});
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

		processMessages();
		
		for (int i = 0; i < m_dimensions.size(); i++) {
			m_dimensions[i]->update(info);
		}

		m_networkStats.updateSample(info.timeStep,this);
		
	}
	void Session::processEvent(bool isServer, engone::NetEvent e, engone::UUID uuid) {
		using namespace engone;
		if (isServer) {
			if (e == NetEvent::Connect) {
				enableMessages();
				m_clients[uuid] = {};
				prounk::GameApp* app = m_parent;
				Dimension* dim = getDimension("0");
				EngineObjectIterator iter = dim->getWorld()->createIterator();
				EngineObject* obj;
				while (obj = iter.next()) {
					netAddGeneral(obj);
					netEditTrigger(obj, obj->isOnlyTrigger());
					netEditBody(obj, (uint64_t)obj->getRigidBody()->getType());
					//netMoveObject(obj->getUUID(), obj->rigidBody->getTransform(),obj->rigidBody->getLinearVelocity(),obj->rigidBody->getAngularVelocity());
				}
			} else if (e == NetEvent::Disconnect) {
				auto find = m_clients.find(uuid);
				if (find == m_clients.end()) {
					// this would be a bug
					log::out << log::RED << "Session::Event - Cannot find client data for " << uuid << "\n";
				} else {
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
							DeleteObject(dim, obj);
						} else {
							log::out << log::RED << "Session::Event - Object is not owned?\n";

						}
					}
					m_clients.erase(find);
				}
			} else if (e == NetEvent::Stopped) {
				disableMessages();
			}
		} else {
			if (e == NetEvent::Connect) {
				prounk::GameApp* app = (prounk::GameApp*)m_parent;

				enableMessages();
				Dimension* dim = getDimension("0");
				EngineObjectIterator iter = dim->getWorld()->createIterator();
				EngineObject* obj;
				while (obj = iter.next()) {
					if (obj->getFlags() & OBJECT_NETMOVE) {
						netAddGeneral(obj);
						netEditTrigger(obj, obj->isOnlyTrigger());
						netEditBody(obj, (uint64_t)obj->getRigidBody()->getType());
					}
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
				Dimension* dim = getDimension("0");
				EngineObjectIterator iter = dim->getWorld()->createIterator();
				EngineObject* obj;
				while (obj = iter.next()) {
					if ((obj->getFlags() & OBJECT_NETMOVE) == 0) {
						DeleteObject(dim, obj);
					}
				}
				disableMessages();
			}
		}
	}
	void Session::processMessage(engone::MessageBuffer* msg, engone::UUID clientUUID) {
		using namespace engone;
		
		// Split this function into sub functions
		
		//log::out << "Receive: Size:" <<msg.size()<<" UUID: "<<uuid<<"\n";
		Sender* sender = m_server.isRunning() ? (Sender*)&m_server : (Sender*)&m_client;

		//void(NetProtocols:: * f[])(MessageBuffer & msg, UUID clientUUID) = {recAddCreature};

		//f[(int)MoveObject](msg, clientUUID);

		NetMessageType cmd;
		msg->pull(&cmd);

		if (cmd != NET_MOVE&&cmd!=NET_DAMAGE) {
			if (sender->isServer()) {
				//log::out <<"Server(" << clientUUID.data[0] << "): " << cmd << "\n";
				log::out <<log::GRAY<<"Server: " << cmd << "\n";
			} else {
				log::out << log::GRAY <<"Client: " << cmd << "\n";
			}
		}

		if (cmd == NET_MOVE) {
			//-- Extract data
			rp3d::Transform transform;
			rp3d::Vector3 velocity;
			rp3d::Vector3 angularVelocity;
			UUID_DIM obj;
			pullObject(msg, obj);
			msg->pull(&transform);
			msg->pull(&velocity);
			msg->pull(&angularVelocity);

			//-- Find object with matching UUID
			if (obj.dim) {
				obj.dim->getWorld()->lockPhysics();
				EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);

				//log::out << (BasicObjectType)object->getObjectType() << "" << velocity << " " << angularVelocity << "\n";

				if (object) {
					object->getRigidBody()->setTransform(transform);
					object->getRigidBody()->setLinearVelocity(velocity);
					object->getRigidBody()->setAngularVelocity(angularVelocity);
					obj.dim->getWorld()->releaseAccess(obj.uuid);
				}
				obj.dim->getWorld()->unlockPhysics();
			}
		} else if (cmd == NET_ADD_TERRAIN) {
			recAddTerrain(msg, clientUUID);
		} else if (cmd == NET_ADD_ITEM) {
			recAddItem(msg, clientUUID);
		} else if (cmd == NET_ADD_WEAPON) {
			recAddWeapon(msg, clientUUID);
		} else if (cmd == NET_ADD_CREATURE) {
			recAddCreature(msg, clientUUID);
		} else if (cmd == NET_ANIMATE) {
			//-- Extract data
			UUID_DIM obj;
			pullObject(msg, obj);

			std::string instance;
			std::string animation;
			bool loop;
			float speed;
			float blend;
			float frame;
			msg->pull(instance);
			msg->pull(animation);
			msg->pull(&loop);
			msg->pull(&speed);
			msg->pull(&blend);
			msg->pull(&frame);

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

		} else if (cmd == NET_EDIT_TRIGGER) {
			//-- Extract data
			UUID_DIM obj;
			pullObject(msg, obj);

			//int type;
			//msg.pull(&type);
			bool yes;
			msg->pull(&yes);

			//-- Find object with matching UUID
			if (obj.dim) {
				obj.dim->getWorld()->lockPhysics();
				EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);
				if (object) {
					object->setOnlyTrigger(yes);
					obj.dim->getWorld()->releaseAccess(obj.uuid);
				}
				obj.dim->getWorld()->unlockPhysics();
			}
		} else if (cmd == NET_EDIT_BODY) {
			//-- Extract data
			UUID_DIM obj;
			pullObject(msg, obj);

			uint64_t type;
			msg->pull(&type);

			//-- Find object with matching UUID
			if (obj.dim) {
				EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);
				if (object) {
					obj.dim->getWorld()->lockPhysics();
					object->getRigidBody()->setType((rp3d::BodyType)type);
					obj.dim->getWorld()->unlockPhysics();
					obj.dim->getWorld()->releaseAccess(obj.uuid);
				}
			}
		} else if (cmd == NET_DAMAGE) {
			recDamageObject(msg, clientUUID);
		} else if (cmd == NET_EDIT_HEALTH) {
			//-- Extract data
			UUID_DIM obj;
			pullObject(msg, obj);

			float health;
			msg->pull(&health);

			//-- Find object with matching UUID
			if (obj.dim) {
				EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);

				if (object) {

					auto& oinfo = objectInfoRegistry.getCreatureInfo(object->getObjectInfo());
					oinfo.combatData.health = health;
					if (oinfo.combatData.health < 0)
						oinfo.combatData.health = 0;

					// Todo: Could dealCombat be changed a bit to call a sub function which this code also calls to avoid code duplication?

					// Todo: Change this to contact point instead of object.pos. Contact point should be added in NET_DAMAGE.
					//getParent()->visualEffects.addDamageParticle(object->getPosition());
					obj.dim->getWorld()->releaseAccess(obj.uuid);
				}
			}
		} else if (cmd == NET_EDIT_MODEL) {
			//-- Extract data
			UUID_DIM obj;
			pullObject(msg, obj);

			std::string modelName;
			msg->pull(modelName);

			//-- Find object with matching UUID
			if (obj.dim) {
				EngineObject* object = obj.dim->getWorld()->requestAccess(obj.uuid);

				if (object) {
					engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
					object->setModel(assets->load<engone::ModelAsset>(modelName));
					obj.dim->getWorld()->releaseAccess(obj.uuid);
				}
			}
		} else if (cmd == NET_DELETE) {
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
	}
	void Session::processMessages() {
		using namespace engone;
		m_messageQueueMutex.lock();
		std::vector<NetMessage> queue = std::move(m_messageQueue);
		m_messageQueueMutex.unlock();

		for (int i = 0; i < queue.size(); i++) {
			NetMessage& msg = queue[i];

			if (msg.isEvent)
				processEvent(msg.isServer, msg.netEvent, msg.uuid);
			else {
				processMessage(msg.msg, msg.uuid);
				ALLOC_DELETE(MessageBuffer, msg.msg);
			}
		}
		
		// queue is cleared
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
	void Session::enableMessages() {
		m_areMessagesEnabled = true;
	}
	void Session::disableMessages() {
		m_areMessagesEnabled = false;
	}
	bool Session::areMessagesEnabled() {
		return m_areMessagesEnabled;
	}
}