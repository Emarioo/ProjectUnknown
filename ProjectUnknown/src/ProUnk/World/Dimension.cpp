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
		CombatData* combatData = GetCombatData(obj);

		if (!combatData) {
			log::out << log::RED << "Dimension : Dummy has null combat data!";
			return;
		}
		//log::out << "Pos " << obj->getPosition() << "\n";
		//-- Is Dummy dead?
		if (combatData->health == 0) {

			DropInventory(obj); // does this happen for dummy on client and server because that may lead to double items.
			// maybe respawn dummy?
			DeleteObject(this, obj);
			return;
		}
		//-- Refresh attacking
		combatData->animationTime = 1.f; // required otherwise attacking will be set to false
		combatData->attack(); // attacks every tick.

		//-- Last damaged
		if (combatData->health != combatData->lastHealth) {
			combatData->lastDamagedSeconds = 0;
			combatData->lastHealth = combatData->health;
		} else {
			combatData->lastDamagedSeconds += info.timeStep;
		}

		//-- Auto heal
		if (combatData->lastDamagedSeconds > 5) {
			combatData->health += 20*info.timeStep;
			if (combatData->health > combatData->getMaxHealth())
				combatData->health = combatData->getMaxHealth();
			combatData->lastHealth = combatData->health;
		}
		
		if (combatData->health < combatData->getMaxHealth() * 0.4) {
			// flee
			combatData->target = 0;

			// find closest player
			float minDist = 9999999;
			EngineObject* plr = nullptr;
			EngineObjectIterator iterator = m_world->createIterator();
			EngineObject* obj2;
			while (obj2 = iterator.next()) {
				if (obj2->getObjectType() == OBJECT_PLAYER) {
					CombatData* combatData = GetCombatData(obj2);
					if (combatData->health != 0) { // player isn't dead
						float leng = glm::length(obj->getPosition() - obj2->getPosition());
						if (leng < minDist || !plr) {
							plr = obj2;
							minDist = leng;
						}
					}
				}
			}

			// Move away from player
			if (plr) {
				glm::vec3 diff = plr->getPosition() - obj->getPosition();
				float length = glm::length(diff);
				float speed = 7.f;
				//if (glm::length(diff) > 10) {
				//	speed *= 1.1;
				//}
				// Todo: Fix this code it doesn't work so well with different UPS.
				glm::vec3 dir = -diff / length;
				glm::vec3 wantedVelocity = speed * dir;
				//glm::vec3 wantedVelocity = speed * dir / (float)info.timeStep;

				obj->getWorld()->lockPhysics();
				glm::vec3 currentVelocity = ToGlmVec3(obj->getRigidBody()->getLinearVelocity());
				obj->getWorld()->unlockPhysics();
				glm::vec3 force = 0.047f * (wantedVelocity - 0.7f * currentVelocity) / (float)info.timeStep;
				//glm::vec3 force = 0.01f*(wantedVelocity - 0.4f*currentVelocity) / info.timeStep;
				//glm::vec3 force = dir * 1.f;
				//log::out << "vel " << glm::length(currentVelocity) << "\n";
				obj->getWorld()->lockPhysics();
				obj->getRigidBody()->applyWorldForceAtCenterOfMass(ToRp3dVec3(force));
				obj->getWorld()->unlockPhysics();
			}
		} else {
			// attack

			// If target is dead, remove target
			if (combatData->target != 0) {
				EngineObject* plr = getWorld()->requestAccess(combatData->target);
				if (plr) {
					CombatData* targetData = GetCombatData(plr);
					if (targetData->health == 0) {
						combatData->target = 0;
					}
					getWorld()->releaseAccess(combatData->target);
				} else {
					combatData->target = 0;
				}
			}

			// If no target, aquire target
			if (combatData->target==0) {
				//float closestScore = GetRandom();
				bool findClosest = GetRandom() < 0.75;
				float bestScore = 0;
				EngineObject* plr = nullptr;
				EngineObjectIterator iterator = m_world->createIterator();
				EngineObject* obj2;
				while (obj2 = iterator.next()) {
					if (obj2->getObjectType() == OBJECT_PLAYER) {
						CombatData* combatData = GetCombatData(obj2);
						if (combatData->health != 0) { // player isn't dead
							float score=0;
							if(findClosest)
								score += glm::length(obj->getPosition() - obj2->getPosition());
								//score += closestScore * glm::length(obj->getPosition() - obj2->getPosition());
							else
								score += combatData->health;
								//score += (1- closestScore) * combatData->health*combatData->health/100;
							if (score < bestScore || !plr) {
								plr = obj2;
								bestScore = score;
							}
						}
					}
				}
				if(plr)
					combatData->target = plr->getUUID();
			}

			// Move towards target
			if (combatData->target != 0) {
				EngineObject* plr = getWorld()->requestAccess(combatData->target);

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
				glm::vec3 force = 0.047f * (wantedVelocity - 0.7f * currentVelocity) / (float)info.timeStep;
				//glm::vec3 force = 0.01f*(wantedVelocity - 0.4f*currentVelocity) / info.timeStep;
				//glm::vec3 force = dir * 1.f;
				//log::out << "vel " << glm::length(currentVelocity) << "\n";
				obj->getWorld()->lockPhysics();
				obj->getRigidBody()->applyWorldForceAtCenterOfMass(ToRp3dVec3(force));
				obj->getWorld()->unlockPhysics();

				getWorld()->releaseAccess(combatData->target);
			}
		}
	}
	void Dimension::SpawnLogic(engone::LoopInfo& info) {
		using namespace engone;
		if (spawnLocations.size() == 0) {
			spawnLocations.push_back({20,0,0});
			spawnLocations.push_back({ -20,0,0 });
			spawnLocations.push_back({ 0,0,20 });
			spawnLocations.push_back({ 0,0,-20 });
		}

		float runtime = info.app->getRuntime();

		spawnTime -= info.timeStep;

		if (spawnTime <= 0) {
			spawnTime += spawnDelay;
			spawnDelay = 6*spawnCount;
			//spawnDelay *= 0.95;
			//if (spawnDelay < 1)
			//	spawnDelay = 1;

			int allocBytes = spawnLocations.size() * sizeof(int);
			int* locationCount = (int*)alloc::malloc(allocBytes);
			ZeroMemory(locationCount,allocBytes);

			int spawn = spawnCount;

			if (getWorld()->getObjectCount() > 50)
				spawn = 0;

			for (int i = 0; i < spawn;i++) {
				int index = GetRandom()*spawnLocations.size();
				locationCount[index]++;
			}
			for (int i = 0; i < spawnLocations.size();i++) {
				for (int j = 0; j < locationCount[i];j++) {
					engone::EngineObject* obj = CreateDummy(this);
					obj->setPosition(spawnLocations[i]+glm::vec3(0,5,0)*(float)j);
					getParent()->netAddGeneral(obj);
				}
			}
			alloc::free(locationCount,allocBytes);

			spawnCount += 1;
		}
	}
	engone::Mutex m_mutex;
	static bool dummyEnabled=false;
	void Dimension::update(engone::LoopInfo& info) {
		using namespace engone;

		//-- Dummy AI (not efficient)
		EngineObjectIterator iterator = m_world->createIterator();
		
		EngineObject* obj;

		if (IsKeyPressed(GLFW_KEY_I)) {
			m_mutex.lock();
			dummyEnabled = !dummyEnabled;
			m_mutex.unlock();
		}

		//SpawnLogic(info);

		while (obj = iterator.next()) {
			if (obj->getPosition().y < KILL_BELOW_Y) {
				if (obj->getObjectType() == OBJECT_PLAYER) {
					if (obj->getFlags() & Session::OBJECT_NETMOVE) { // this client is responsible for the player
						Session* session = getParent();
						auto& oinfo = session->objectInfoRegistry.getCreatureInfo(obj->getObjectInfo());
						oinfo.combatData.health = 0;
						session->netEditHealth(obj, 0);

						//Session* session = getParent();
						//auto combatData = GetCombatData(session, obj);
						//combatData->health = 0;
					}
				} else {
					//DeleteObject(this, obj);
					// no need to send netDelete because the other clients will probably run this code too.
					// if they don't, this client doesn't really care
				}
			}

			if (obj->getObjectType() != OBJECT_DUMMY)
				continue;
			m_mutex.lock();
			if (dummyEnabled) {
				m_mutex.unlock();
				if(obj->getRigidBody()->getLinearDamping()!=0)
					obj->getRigidBody()->setLinearDamping(0);
				if (obj->getRigidBody()->getAngularDamping() != 0)
					obj->getRigidBody()->setAngularDamping(0);
				DummyLogic(info, obj);
			} else {
				m_mutex.unlock();
				GetCombatData(obj)->target = 0;
				if (obj->getRigidBody()->getLinearDamping() != 2)
					obj->getRigidBody()->setLinearDamping(2);
				if (obj->getRigidBody()->getAngularDamping() != 1)
					obj->getRigidBody()->setAngularDamping(1);
			}
		}
		
		//-- Send position to server or client
		if (getParent()->getServer().isRunning() || getParent()->getClient().isRunning()) {
			while (obj = iterator.next()) {
				if (obj->getFlags() & Session::OBJECT_NETMOVE) {
					//log::out << (BasicObjectType)obj->getObjectType() << "\n";
					getParent()->netMoveObject(obj);
					//netMoveObject(object->getUUID(), object->rigidBody->getTransform(),
					//	object->rigidBody->getLinearVelocity(), object->rigidBody->getAngularVelocity());
				}
			}
		}
		// combatdata
		//iterator->restart();
		while (obj = iterator.next()) {
			CombatData* combatData = GetCombatData(obj);
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