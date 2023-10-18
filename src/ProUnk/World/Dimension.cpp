#include "ProUnk/World/Dimension.h"

#include "ProUnk/World/Session.h"
// #include "ProUnk/GameApp.h"

#include "ProUnk/Objects/BasicObjects.h"

#include "Engone/Engone.h"

namespace prounk {
	void Dimension::dealCombat(engone::EngineObject* atkObj, engone::EngineObject* defObj, glm::vec3 contactPoint) {
		using namespace engone;

		int atkId = atkObj->getObjectInfo();
		int defId = defObj->getObjectInfo();
		if (!atkId || !defId){
			log::out << log::RED << "GameApp::dealCombat - one of the object's id are 0\n";
			return;
		}

		// Todo: will crash if no dimension. not good but i want to get this stuff done
		//Dimension* dim = m_session->getDimensions()[0];

		CombatData* atkData = GetCombatData(atkObj);
		CombatData* defData = GetCombatData(defObj);
		
		if (!atkData || !defData) {
			log::out << log::RED << "GameApp::dealCombat - atkData or defData is nullptr\n";
			return;
		}

		if (!atkData->attacking) return;

		if (IsObject(atkObj, OBJECT_DUMMY) && IsObject(defObj, OBJECT_DUMMY))
			return;

		if (!(atkObj->getFlags() & Session::OBJECT_NETMOVE)) // ensure collision is done once, one of the clients or server not all of them.
			return;
		// can't attack themself
		if (atkData->owner == defObj) {
			return;
		}

		// skip if object has been hit earlier in the attack
		bool hit = false;
		for (int i=0;i<atkData->hitObjects.size();i++) {
			auto& uuid = *(engone::UUID*)atkData->hitObjects.get(i);
			if (uuid == defObj->getUUID())
				hit = true;
		}
		if (hit)
			return;

		// atkData->hitObjects.push_back(defObj->getUUID());
		atkData->hitObjects.add(&defObj->getUUID());
		

		float atk=0;
		if (atkData->damageType == CombatData::CONTINOUS_DAMAGE) {
			atk = atkData->damagePerSecond*getParent()->getParent()->getLoopInfo().timeStep;
		} else if (atkData->damageType == CombatData::SINGLE_DAMAGE) {
			atk = atkData->singleDamage;
		}
		
		if (defData->getHealth() == 0)
			return;

		defData->setHealth(defData->getHealth() - atk);
		if (defData->getHealth() < 0)
			defData->setHealth(0);
		// send updated health.
		// what if someone else also updates health? then do addition or subtraction on health.
		// how about hit cooldown? can other players deal damage two you at the same time?
		// i'm thinking yes.
		// still need cooldown because the same player should not be able to deal damage from the same attack twice.
		// instead of cooldown have one frame were attack deals damage? bad if frame is skipped, it won't be skipped because of how
		// the game loop works. delta is fixed. it may skip one or two frames though, depending on speed and stuff.
		// You could ensure this doesn't happen? the attack chould store which objects you hit. and then
		// don't deal damage to th+em. you can deal damage between an interval of frames.
		// CombatData requires vector.
		// 
		// let's not focus to much on cheating
		// 

		//glm::vec3 particlePosition = ToGlmVec3(pair.getCollider1()->getLocalToWorldTransform().getPosition() +
		//	pair.getCollider2()->getLocalToWorldTransform().getPosition()) / 2.f;
		//glm::vec3 particlePosition = defObj->getPosition();

		// spawn particles
		// hit cooldown is determined by the attacker's animation time left
		//doParticles(defObj->getPosition());
		// visualEffects.addDamageParticle(contactPoint);
		//CombatParticle* parts = combatParticles->getParticles();
		//for (int i = 0; i < combatParticles->getCount(); i++) {
		//	//float rad = glm::pi<float>() * (pow(2, 3) - pow(2 * GetRandom(), 3)) / 3.f;
		//	float rad = GetRandom() * 0.5;
		//	glm::vec3 norm = glm::vec3(GetRandom() - 0.5f, GetRandom() - 0.5f, GetRandom() - 0.5f);
		//	norm = glm::normalize(norm);
		//	parts[i].pos = particlePosition + norm * rad;
		//	parts[i].vel = norm * (0.1f + (float)GetRandom()) * 1.f;
		//	parts[i].lifeTime = 1 + GetRandom() * 3.f;
		//}

		char buffer[15];
		snprintf(buffer, sizeof(buffer), "%.1f", atk);
		//glm::vec4 numColor = { GetRandom(),GetRandom(),GetRandom(),1 };
		glm::vec4 numColor = {1, 0.1, 0.05, 1};
		// visualEffects.addDamageNumber(buffer, contactPoint, numColor);

		// NOTE: if the attack anim. is restarted when the attack collider still is in the defense collider the cooldown would still be active
		//	Meaning no damage.

		glm::vec3 knockDirection{0,0,0};
		
		float timeStep = getParent()->getParent()->getLoopInfo().timeStep;
		float knockStrength = atkData->knockStrength;
		glm::vec3 diff = (defData->owner->getPosition() - atkData->owner->getPosition());
		float leng = glm::length(diff);
		if (leng != 0) {
			glm::vec3 dir = diff / leng;
			glm::vec3 velocity = knockStrength * dir / timeStep;
			velocity += atkData->owner->getLinearVelocity() - defData->owner->getLinearVelocity();

			//rp3d::Vector3 rot = ToEuler(atkData->owner->rigidBody->getTransform().getOrientation());
			//rp3d::Vector3 force = { glm::sin(rot.y),0,glm::cos(rot.y) };
			//log::out << force << "\n";
			//rp3d::Vector3 force = def.body->getTransform().getPosition() - atk.body->getTransform().getPosition();
			//force.y = 1;
			knockDirection = velocity / timeStep;
			//log::out << "Apply force " << force << "\n";
			//for now, defense can be assumed to be a rigidbody
			defObj->applyForce(knockDirection);
		}

		getParent()->netDamageObject(defObj, atk, knockDirection);
	}
	void Dimension::onContact(const rp3d::CollisionCallback::CallbackData& callbackData) {
		using namespace engone;
		for (int pairI = 0; pairI < callbackData.getNbContactPairs(); pairI++) {
			auto pair = callbackData.getContactPair(pairI);

			EngineObject* obj1 = (EngineObject*)pair.getBody1()->getUserData();
			EngineObject* obj2 = (EngineObject*)pair.getBody2()->getUserData();
			//log::out << getGround()->getClient().isRunning() << " " << getGround()->getClient().isRunning() <<" "<< pair.getBody1() << " " << pair.getBody2() << "\n";

			//if (pair.getEventType() == rp3d::CollisionCallback::ContactPair::EventType::ContactStart) {
			//	s_test1++;
			//	log::out << "start1 " << s_test1 << "\n";
			//} else if (pair.getEventType() == rp3d::CollisionCallback::ContactPair::EventType::ContactExit) {
			//	s_test1--;
			//	log::out << "stop1 " << s_test1 << "\n";
			//}

			//log::out << "Contact: " << (int)pair.getEventType() << "\n";
			if (!obj1 || !obj2) continue;
			
			//if (pair.getEventType() == rp3d::CollisionCallback::ContactPair::EventType::ContactStart) {
			if (IsObject(obj1,OBJECT_CREATURE)) {
				auto oinfo = ((Dimension*)obj1->getWorld()->getUserData())->getParent()->objectInfoRegistry.getCreatureInfo(obj1->getObjectInfo());
				oinfo->onGround=true;
				//log::out << "start1 " << oinfo.onGround << "\n";
			}
			if (IsObject(obj2,OBJECT_CREATURE)) {
				auto oinfo = ((Dimension*)obj2->getWorld()->getUserData())->getParent()->objectInfoRegistry.getCreatureInfo(obj2->getObjectInfo());
				oinfo->onGround=true;
				//log::out << "start2 " << oinfo.onGround << "\n";
			}
			//} else if (pair.getEventType() == rp3d::CollisionCallback::ContactPair::EventType::ContactExit) {
			//	if (obj1->getObjectType() & OBJECT_CREATURE) {
			//		auto& oinfo = ((Dimension*)obj1->getWorld()->getUserData())->getParent()->objectInfoRegistry.getCreatureInfo(obj1->getObjectInfo());
			//		oinfo.onGround--;
			//		log::out << "stop1 " << oinfo.onGround << "\n";
			//	}
			//	if (obj2->getObjectType() & OBJECT_CREATURE) {
			//		auto& oinfo = ((Dimension*)obj2->getWorld()->getUserData())->getParent()->objectInfoRegistry.getCreatureInfo(obj2->getObjectInfo());
			//		oinfo.onGround--;
			//		log::out << "stop2 " << oinfo.onGround << "\n";
			//	}
			//}
			//engone::EngineObject* plr = nullptr;
			//if (obj1->getObjectType() == OBJECT_PLAYER) {
			//	plr = obj1;
			//}
			//if (obj2->getObjectType() == OBJECT_PLAYER) {
			//	plr = obj2;
			//}
			//if (plr) {
			//	auto oinfo = ((Dimension*)plr->getWorld()->getUserData())->getParent()->objectInfoRegistry.getCreatureInfo(plr->getObjectInfo());
			//	log::out << "ground: " << oinfo.onGround << "\n";
			//}
			if (pair.getEventType() == rp3d::CollisionCallback::ContactPair::EventType::ContactExit)
				continue;
		
			// Checking for combat data instead of creature because a barrel could be broken but isn't a creature.
			if (HasCombatData(obj1->getObjectType()) && HasCombatData(obj2->getObjectType())) {
				//printf("%d - %d\n", (uint32_t)pair.getCollider1()->getUserData(), (uint32_t)pair.getCollider2()->getUserData());
				int count = pair.getNbContactPoints();

				glm::vec3 contactPoint;
				if (count != 0) {
					auto cp = pair.getContactPoint(0);
					rp3d::Vector3 vec = pair.getCollider1()->getLocalToWorldTransform() * cp.getLocalPointOnCollider1();
					contactPoint = ToGlmVec3(vec);
				} else {
					rp3d::Vector3 mid = (pair.getBody1()->getTransform().getPosition() + pair.getBody2()->getTransform().getPosition()) / 2;
					contactPoint = ToGlmVec3(mid);
				}

				if (((u64)pair.getCollider1()->getUserData() & COLLIDER_IS_DAMAGE) && ((u64)pair.getCollider2()->getUserData() & COLLIDER_IS_HEALTH))
					dealCombat(obj1, obj2, contactPoint);
				if (((u64)pair.getCollider1()->getUserData() & COLLIDER_IS_HEALTH) && ((u64)pair.getCollider2()->getUserData() & COLLIDER_IS_DAMAGE))
					dealCombat(obj2, obj1, contactPoint);
			}
		}
	}
	void Dimension::onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) {
		using namespace engone;
		//log::out << "TRIGGAR\n";
		for (int pairI = 0; pairI < callbackData.getNbOverlappingPairs(); pairI++) {
			auto pair = callbackData.getOverlappingPair(pairI);
			//pair.getEventType()
			//continue;
			//auto pair = pair.getEventType();
			//rp3d::OverlapCallback::OverlapPair::EventType::
			
			EngineObject* obj1 = (EngineObject*)pair.getBody1()->getUserData();
			EngineObject* obj2 = (EngineObject*)pair.getBody2()->getUserData();

			//log::out << getGround()->getClient().isRunning() << " " << getGround()->getClient().isRunning() <<" "<< pair.getBody1() << " " << pair.getBody2() << "\n";

			if (!obj1 || !obj2) continue;
			
			if (pair.getEventType() != rp3d::OverlapCallback::OverlapPair::EventType::OverlapExit) {
				if (obj1->getObjectType() != obj2->getObjectType()) {
					if (IsObject(obj1, OBJECT_TRIGGER)) {
						auto oinfo = ((Dimension*)obj1->getWorld()->getUserData())->getParent()->objectInfoRegistry.getTriggerInfo(obj1->getObjectInfo());
						if(!oinfo->hit(obj2->getUUID()))
							oinfo->collisions.push_back(obj2->getUUID());
					}
					if (IsObject(obj2, OBJECT_TRIGGER)) {
						auto oinfo = ((Dimension*)obj2->getWorld()->getUserData())->getParent()->objectInfoRegistry.getTriggerInfo(obj2->getObjectInfo());
						if (!oinfo->hit(obj1->getUUID()))
							oinfo->collisions.push_back(obj1->getUUID());
					}
				}
			}

			if (HasCombatData(obj1->getObjectType()) && HasCombatData(obj2->getObjectType())) {
				rp3d::Vector3 cp2 = (pair.getCollider1()->getLocalToWorldTransform().getPosition() + pair.getCollider2()->getLocalToWorldTransform().getPosition()) / 2;
				//rp3d::Vector3 cp2 = (pair.getBody1()->getTransform().getPosition() + pair.getBody2()->getTransform().getPosition()) / 2;
				//rp3d::Vector3 cp2 = (pair.getBody1()->getTransform().getPosition() + pair.getBody2()->getTransform().getPosition()) / 2;
				glm::vec3 contactPoint = ToGlmVec3(cp2);
				//printf("%d - %d\n", (uint32_t)pair.getCollider1()->getUserData(), (uint32_t)pair.getCollider2()->getUserData());
				if(((u64)pair.getCollider1()->getUserData() & COLLIDER_IS_DAMAGE) && ((u64)pair.getCollider2()->getUserData() & COLLIDER_IS_HEALTH))
					dealCombat(obj1, obj2, contactPoint);
				if (((u64)pair.getCollider1()->getUserData() & COLLIDER_IS_HEALTH) && ((u64)pair.getCollider2()->getUserData() & COLLIDER_IS_DAMAGE))
					dealCombat(obj2, obj1, contactPoint);
			}
		}
	}
	Dimension::Dimension(Session* session) : m_parent(session) {
		m_world = session->getParent()->createWorld();
		m_world->setUserData(this);
		
		
		m_world->getPhysicsWorld()->setEventListener(this);
		
		glm::vec3 dungeonPosition = {73,-8,0};
		dungeon.init(this, dungeonPosition+glm::vec3{ 20,0,0 }, dungeonPosition + glm::vec3{-20,0,0});
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
		if (combatData->getHealth() == 0) {

			DropInventory(obj); // does this happen for dummy on client and server because that may lead to double items.
			// maybe respawn dummy?
			getParent()->netDeleteObject(obj);
			DeleteObject(this, obj);
			return;
		}
		//-- Refresh attacking
		combatData->animationTime = 1.f; // required otherwise attacking will be set to false
		combatData->attack(); // attacks every tick.

		//-- Last damaged
		if (combatData->getHealth() != combatData->lastHealth) {
			combatData->lastDamagedSeconds = 0;
			combatData->lastHealth = combatData->getHealth();
		} else {
			combatData->lastDamagedSeconds += info.timeStep;
		}

		//-- Auto heal
		if (combatData->lastDamagedSeconds > 5) {
			if (combatData->getHealth() < combatData->getMaxHealth()) {
				combatData->setHealth(combatData->getHealth() + 20 * info.timeStep);
				if (combatData->getHealth() > combatData->getMaxHealth())
					combatData->setHealth(combatData->getMaxHealth());
				combatData->lastHealth = combatData->getHealth();
				getParent()->netEditHealth(obj, combatData->getHealth());
			}
		}
		
		if (combatData->getHealth() < combatData->getMaxHealth() * 0.35) {
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
					if (combatData->getHealth() != 0) { // player isn't dead
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
				float speed = 2.f;
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
					if (targetData->getHealth() == 0) {
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
						if (combatData->getHealth() != 0) { // player isn't dead
							float score=0;
							if(findClosest)
								score += glm::length(obj->getPosition() - obj2->getPosition());
								//score += closestScore * glm::length(obj->getPosition() - obj2->getPosition());
							else
								score += combatData->getHealth();
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

		// float runtime = info.app->getRuntime();

		spawnTime -= info.timeStep;

		if (spawnTime <= 0) {
			spawnTime += spawnDelay;
			spawnDelay = 5;
			//spawnDelay *= 0.95;
			//if (spawnDelay < 1)
			//	spawnDelay = 1;

			int allocBytes = spawnLocations.size() * sizeof(int);
			int* locationCount = (int*)Allocate(allocBytes);
			ZeroMemory(locationCount,allocBytes);

			int spawn = spawnCount;

			//if (getWorld()->getObjectCount() > 50)
			//	spawn = 0;

			for (int i = 0; i < spawn;i++) {
				int index = GetRandom()*spawnLocations.size();
				locationCount[index]++;
			}
			for (int i = 0; i < spawnLocations.size();i++) {
				for (int j = 0; j < locationCount[i];j++) {
					engone::EngineObject* obj = CreateDummy(this);
					obj->setPosition(spawnLocations[i]+glm::vec3(0,5,0)*(float)j);
					getParent()->netAddGeneral(obj);
					getParent()->netMoveObject(obj);
				}
			}
			Free(locationCount,allocBytes);

			spawnCount += 1;
		}
	}
	struct Temp {
		bool dummyEnabled = false;
		bool spawningEnabled = false;
	};
	// std::unordered_map<engone::Application*, Temp> enables;
	void Dimension::update(engone::LoopInfo* info) {
		using namespace engone;

		//-- Dummy AI (not efficient)
		EngineObjectIterator iterator = m_world->createIterator();
		
		EngineObject* obj;

		// if (IsKeyPressed(GLFW_KEY_I)) {
		// 	Temp& temp = enables[info.app];
		// 	temp.dummyEnabled = !temp.dummyEnabled;
		// }
		// if (IsKeyPressed(GLFW_KEY_J)) {
		// 	Temp& temp = enables[info.app];
		// 	temp.spawningEnabled = !temp.spawningEnabled;
		// }

		//if (getParent()->getServer().isRunning()) {
			// Temp& temp = enables[info.app];
			// if(temp.spawningEnabled)
			// 	SpawnLogic(info);
		//}

		while (obj = iterator.next()) {
			if (obj->getPosition().y < KILL_BELOW_Y) {
				if (obj->getObjectType() == OBJECT_PLAYER) {
					if (obj->getFlags() & Session::OBJECT_NETMOVE) { // this client is responsible for the player
						Session* session = getParent();
						auto oinfo = session->objectInfoRegistry.getCreatureInfo(obj->getObjectInfo());
						oinfo->combatData.setHealth(0);
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

			// Temp& temp = enables[info.app];
		//}
			// if (temp.dummyEnabled) {
			// 	//m_mutex.unlock();
			// 	if(obj->getRigidBody()->getLinearDamping()!=0)
			// 		obj->getRigidBody()->setLinearDamping(0);
			// 	if (obj->getRigidBody()->getAngularDamping() != 0)
			// 		obj->getRigidBody()->setAngularDamping(0);
			// 	DummyLogic(info, obj);
			// } else {
			// 	//m_mutex.unlock();
			// 	GetCombatData(obj)->target = 0;
			// 	if (obj->getRigidBody()->getLinearDamping() != 2)
			// 		obj->getRigidBody()->setLinearDamping(2);
			// 	if (obj->getRigidBody()->getAngularDamping() != 1)
			// 		obj->getRigidBody()->setAngularDamping(1);
			// }
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
		
		dungeon.update(info); // Note: do this before clearing collisions!

		// combatdata
		//iterator->restart();
		while (obj = iterator.next()) {
			if (obj->getObjectType() == OBJECT_TRIGGER) {
				auto oinfo = ((Dimension*)obj->getWorld()->getUserData())->getParent()->objectInfoRegistry.getTriggerInfo(obj->getObjectInfo());
				oinfo->collisions.clear();
				continue;
			}
			
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

			if (obj->getObjectType() & OBJECT_CREATURE) {
				auto oinfo = ((Dimension*)obj->getWorld()->getUserData())->getParent()->objectInfoRegistry.getCreatureInfo(obj->getObjectInfo());
				oinfo->onGround = false;
				//log::out << "start2 " << oinfo.onGround << "\n";
			}
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