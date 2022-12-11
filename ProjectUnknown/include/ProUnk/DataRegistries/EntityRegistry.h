#pragma once

#include "ProUnk/DataRegistries/MasterRegistry.h"

#include "ProUnk/Combat/CombatData.h"

namespace prounk {

	class EntityRegistry : public DataRegistry {
	public:
		EntityRegistry() : DataRegistry("entity_registry") {}

		void serialize();
		void deserialize();

		struct Entry {
			CombatData* combatData=nullptr;
			int inventoryIndex = 0;
		};
		Entry& getEntry(int id);
		int addEntry();
		
	private:

		std::vector<Entry> m_entries;

	};
}