#pragma once

#include "Engone/EngineObject.h"

namespace prounk {
	class Dimension;
	class Dungeon {
	public:
		Dungeon() = default;
		~Dungeon() { cleanup(); }
		void cleanup();

		void init(Dimension* dimension, glm::vec3 pos0, glm::vec3 pos1);

		void update(engone::LoopInfo& info);

	private:
		
		// Todo: Switch to uuids?
		engone::EngineObject* entrance0=nullptr;
		engone::EngineObject* entrance1=nullptr;
		
		engone::EngineObject* trigger0=nullptr;
		engone::EngineObject* trigger1=nullptr;

		Dimension* m_dimension=nullptr;
		
		//-- Temporary, not used anymore, delete it
		struct ObjVel {
			engone::EngineObject* obj;
			glm::vec3 vel;
			int skips = 2;
		};
		std::vector<ObjVel> objvels;
	};
}