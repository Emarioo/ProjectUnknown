#pragma once

#include "Engone/Registries/MasterRegistry.h"
#include "Engone/World/EngineWorld.h"

namespace prounk {

	class WorldRegistry : public Registry {
	public:
		WorldRegistry() : Registry("world_registry") {}

		void serialize() override;
		void deserialize() override;

		

	private:

		std::vector<EngineWorld*> worlds;
	};
}