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


	private:
		std::string name; // name or type of dimension
		engone::EngineWorld* m_world=nullptr;
		Session* m_parent = nullptr;

		void DummyLogic(engone::LoopInfo& info, engone::EngineObject* obj);

		friend class Session;
	};
}