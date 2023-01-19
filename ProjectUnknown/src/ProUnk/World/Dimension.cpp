#include "ProUnk/World/Dimension.h"

#include "ProUnk/World/Session.h"
#include "ProUnk/GameApp.h"

namespace prounk {
	Dimension::Dimension(Session* session) : m_parent(session) {
		m_world = session->getParent()->createWorld();
		m_world->setUserData(this);
	}
	engone::EngineWorld* Dimension::getWorld() {
		return m_world;
	}
	Session* Dimension::getParent() {
		return m_parent;
	}
	const std::string& Dimension::getName() {
		return name;
	}
	void Dimension::DummyLogic(engone::LoopInfo& info, engone::EngineObject* obj) {
		using namespace engone;
		CombatData* combatData = GetCombatData(getParent(), obj);

		if (!combatData) {
			log::out << log::RED << "Dimension : Dummy has null combat data!";
			return;
		}

		//-- Is Dummy dead?
		if (combatData->health == 0) {
			// maybe respawn dummy?
			DeleteObject(this, obj);
			return;
		}
		//-- Refresh attacking
		combatData->animationTime = 1.f; // required otherwise attacking will be set to false
		combatData->attack(); // attacks every tick.

		//-- Move towards closest player
		float minDist = 9999999;
		EngineObject* plr = nullptr;
		EngineObjectIterator iterator = m_world->createIterator();
		EngineObject* obj2;
		while (obj2 = iterator.next()) {
			if (obj2->getObjectType() == OBJECT_PLAYER) {
				if ((obj2->getFlags() & OBJECT_IS_DEAD) == 0) {
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
			float length = glm::length(diff);
			float speed = 10.f;
			if (glm::length(diff) > 10) {
				speed *= 2;
			}
			// Todo: Fix this code it doesn't work so well with different UPS.
			glm::vec3 dir = diff / length;
			glm::vec3 wantedVelocity = speed * dir;
			//glm::vec3 wantedVelocity = speed * dir / (float)info.timeStep;

			obj->getWorld()->lockPhysics();
			glm::vec3 currentVelocity = ToGlmVec3(obj->getRigidBody()->getLinearVelocity());
			obj->getWorld()->unlockPhysics();
			glm::vec3 force = 0.047f*(wantedVelocity - 0.7f * currentVelocity) / (float)info.timeStep;
			//glm::vec3 force = 0.01f*(wantedVelocity - 0.4f*currentVelocity) / info.timeStep;
			//glm::vec3 force = dir * 1.f;
			//log::out << "vel " << glm::length(currentVelocity) << "\n";
			obj->getWorld()->lockPhysics();
			obj->getRigidBody()->applyWorldForceAtCenterOfMass(ToRp3dVec3(force));
			obj->getWorld()->unlockPhysics();
		}
	}
	void Dimension::update(engone::LoopInfo& info) {
		using namespace engone;

		//-- Dummy AI (not efficient)
		EngineObjectIterator iterator = m_world->createIterator();
		
		EngineObject* obj;

		while (obj = iterator.next()) {
			if (obj->getObjectType() != OBJECT_DUMMY)
				continue;

			DummyLogic(info, obj);
		}
		
		//-- Send position to server or client
		if (getParent()->getServer().isRunning() || getParent()->getClient().isRunning()) {
			while (obj = iterator.next()) {
				if (obj->getFlags() & Session::OBJECT_NETMOVE) {
					getParent()->netMoveObject(obj);
					//netMoveObject(object->getUUID(), object->rigidBody->getTransform(),
					//	object->rigidBody->getLinearVelocity(), object->rigidBody->getAngularVelocity());
				}
			}
		}
		// combatdata
		//iterator->restart();
		while (obj = iterator.next()) {
			CombatData* combatData = GetCombatData(getParent(), obj);
			if (!combatData)
				continue;

			// wasUpdated is temporary. objects may share combatData and it may therefore be updated multiple times.
			// this prevents that. A permanant solution would be to have combatData in an contigouous list
			// which you can loop through. CombatData will not be updated twice and it is better for cache.
			//if (!combatData->wasUpdated) {
				combatData->update(info);
			//	combatData->wasUpdated = true;
			//}
		}

		//m_world->deleteIterator(iterator);
		//while (obj = iterator->next()) {
		//	if (obj->getFlags() & OBJECT_HAS_COMBATDATA) {
		//		CombatData* combatData = entityRegistry.getEntry(obj->userData).combatData;
		//		combatData->wasUpdated = false;
		//	}
		//}
	}
}