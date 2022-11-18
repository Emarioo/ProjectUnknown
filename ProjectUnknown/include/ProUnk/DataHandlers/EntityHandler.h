#pragma once

#include "ProUnk/DataHandlers/DataHandlerRegistry.h"

#include "ProUnk/Combat/CombatData.h"

namespace prounk {

	class EntityHandler : public DataHandler {
	public:
		EntityHandler() : DataHandler("entity_handler") {}

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