#pragma once

#include "Engone/World/EngineWorld.h"

namespace prounk {

	class Session;
	class Dimension {
	public:
		Dimension(Session* session);

		engone::EngineWorld* getWorld();

		Session* getParent();

		void update(engone::LoopInfo& info);

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

		void SpawnLogic(engone::LoopInfo& info);
		void DummyLogic(engone::LoopInfo& info, engone::EngineObject* obj);

		friend class Session;
	};
}