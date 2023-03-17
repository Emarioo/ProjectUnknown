#include "ProUnk/World/Dungeon.h"

// #include "ProUnk/GameApp.h"

#include "ProUnk/Objects/BasicObjects.h"

namespace prounk {
	void Dungeon::cleanup() {
		using namespace engone;
		log::out << log::RED << "Dungeon : Missing cleanup\n";
	}

	void Dungeon::init(Dimension* dimension, glm::vec3 pos0, glm::vec3 pos1) {
		//return;
		trigger0 = CreateTrigger(dimension, {1,1,1});
		trigger1 = CreateTrigger(dimension, {1,1,1});

		auto we = trigger0->getRigidBody()->getCollider(0);

		float diff = 2;

		trigger0->setPosition(pos0 + glm::vec3(0,0,diff));
		trigger1->setPosition(pos1 + glm::vec3(0,0,-diff));

		entrance0 = CreateTerrain(dimension, "DEntrance/DEntrance");
		entrance1 = CreateTerrain(dimension, "DEntrance/DEntrance");
		
		entrance0->setPosition(pos0);
		entrance1->setPosition(pos1);
		
		m_dimension = dimension;
	}
	void ResetDownVelocity(engone::EngineObject* obj) {
		//glm::vec3 vec = obj->getLinearVelocity();
		//vec.y = 0.1;
		//obj->setLinearVelocity(vec);
		obj->setLinearVelocity({});
	}
	void Dungeon::update(engone::LoopInfo* info) {
		using namespace engone;
		//for (int i = 0; i < objvels.size();i++) {
		//	auto& a = objvels[i];
		//	a.skips--;
		//	if (a.skips == 0) {
		//		a.obj->setLinearVelocity(a.vel);
		//		objvels.erase(objvels.begin() + i);
		//		i--;
		//	}			
		//}

		//glm::vec3 up = { 0,0.1,0 };
		glm::vec3 up = { };
		glm::vec3 e0to1;
		if(entrance0&&entrance1)
			e0to1 = entrance1->getPosition() - entrance0->getPosition();
		//log::out << "eh " << e0to1 << "\n";
		if (trigger0) {
			//Session* session = ((Dimension*)trigger0->getWorld()->getUserData())->getParent();
			//auto& oinfo = session->objectInfoRegistry.getTriggerInfo(trigger0->getObjectInfo());
			//log::out << "info "<<trigger0->getObjectInfo() << "\n";
			auto oinfo = GetTriggerInfo(trigger0);
			for (auto& u : oinfo->collisions) { // Todo: Bug in multiplayer if you add to many swords
				auto obj = m_dimension->getWorld()->requestAccess(u);
				if (obj) {
					//if (obj->getObjectType() == OBJECT_PLAYER) {
					if (IsObject(obj, OBJECT_CREATURE)) {
						obj->setPosition(obj->getPosition()+e0to1 + up);
						//objvels.push_back({ obj,obj->getLinearVelocity() });
						ResetDownVelocity(obj);
						//obj->getRigidBody()->resetForce();
						//glm::vec3 vec = obj->getLinearVelocity();
						//obj->setLinearVelocity(vec);
					}
				}
				//log::out << "col0 " << u << "\n";
			}
		}
		if (trigger1) {
			auto oinfo = GetTriggerInfo(trigger1);
			for (auto& u : oinfo->collisions) {
				auto obj = m_dimension->getWorld()->requestAccess(u);
				if (obj) {
					if (IsObject(obj,OBJECT_CREATURE)) {
					//if (obj->getObjectType() == OBJECT_PLAYER) {
						obj->setPosition(obj->getPosition() - e0to1 + up);
						//objvels.push_back({ obj,obj->getLinearVelocity() });
						ResetDownVelocity(obj);
						//obj->getRigidBody()->resetForce();
						//glm::vec3 vec = obj->getLinearVelocity();
						//obj->setLinearVelocity(vec);
					}
				}
				//log::out << "col1 " << u << "\n";
			}
		}
	}
}