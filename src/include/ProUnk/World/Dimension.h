#pragma once

#include "Engone/World/EngineWorld.h"
#include "ProUnk/World/Dungeon.h"

#include "Engone/Utilities/rp3d.h"

namespace prounk {

	class Session;
	class Dimension : rp3d::EventListener {
	public:
		Dimension(Session* session);

		engone::EngineWorld* getWorld();

		void onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) override;
		void onContact(const rp3d::CollisionCallback::CallbackData& callbackData) override;
		void dealCombat(engone::EngineObject* atkObj, engone::EngineObject* collider, glm::vec3 contactPoint);


		Session* getParent();

		void update(engone::LoopInfo* info);

		const std::string& getName();

	private:
		static constexpr float KILL_BELOW_Y = -100;
		std::string name; // name or type of dimension, needs to be unique
		engone::EngineWorld* m_world=nullptr;
		Session* m_parent = nullptr;

		// temporary
		std::vector<glm::vec3> spawnLocations;
		int spawnCount=1;
		float spawnTime=0;
		float spawnDelay=1;

		Dungeon dungeon;

		void SpawnLogic(engone::LoopInfo& info);
		void DummyLogic(engone::LoopInfo& info, engone::EngineObject* obj);

		friend class Session;
	};
}