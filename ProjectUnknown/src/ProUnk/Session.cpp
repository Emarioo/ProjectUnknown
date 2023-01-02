#include "ProUnk/Session.h"

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

	}
	std::vector<Dimension*>& Session::getDimensions() {
		return m_dimensions;
	}
	Dimension* Session::createDimension() {
		Dimension* dim = ALLOC_NEW(Dimension)(this);
		
		m_dimensions.push_back(dim);
		return dim;
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

	void NetworkFuncs::netMoveObject(engone::EngineObject* object) {
		using namespace engone;
		if (!networkRunning()) {
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
		rp3d::Vector3 vel = object->getRigidBody()->getLinearVelocity();
		rp3d::Vector3 angVel = object->getRigidBody()->getAngularVelocity();
		msg.push(&uuid);
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

		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = AddObject;
		msg.push(cmd);
		msg.push(&uuid);

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

		//-- Prepare message
		MessageBuffer msg;
		NetCommand cmd = DELETE_OBJECT;
		msg.push(cmd);
		msg.push(&uuid);

		//-- Send message
		networkSend(msg);
	}
	void NetworkFuncs::netAnimateObject(engone::UUID uuid, const std::string& instance, const std::string& animation, bool loop, float speed, float blend, float frame) {
		using namespace engone;
		if (!networkRunning()) {
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
		networkSend(msg);
	}
	void NetworkFuncs::netEditObject(engone::UUID uuid, int type, uint64_t data) {
		using namespace engone;
		if (!networkRunning()) {
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
		networkSend(msg);
	}
	void NetworkFuncs::netEditCombatData(engone::UUID uuid, engone::UUID player, bool yes) {
		using namespace engone;
		if (!networkRunning()) {
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
		networkSend(msg);
	}
	void NetworkFuncs::netDamageObject(engone::UUID uuid, float damage) {
		using namespace engone;
		if (!networkRunning()) {
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