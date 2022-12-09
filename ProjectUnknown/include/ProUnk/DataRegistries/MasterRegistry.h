#pragma once

#include "Engone/Logger.h"
#include "Engone/Utilities/FileUtility.h"

namespace prounk {
	typedef int RegistryId;

	class DataRegistry {
	public:
		DataRegistry(const char* name) : m_name(name) {}

		virtual void serialize() = 0;
		virtual void deserialize() = 0;

		const char* getName() { return m_name; };
		RegistryId getId() { return m_id; };

	private:

		RegistryId m_id;
		const char* m_name;

		friend class MasterRegistry;
	};
	/*
		handler id 0 is seen as nullptr.
	*/
	class MasterRegistry {
	public:
		MasterRegistry() = default;

		void serialize();
		void deserialize();

		// returns nullptr if id doesn't exist or if handler hasn't been registered
		DataRegistry* getRegistry(RegistryId id);
		// returns nullptr if name doesn't exist or if handler hasn't been registered
		DataRegistry* getRegistry(const std::string& name);

		RegistryId registerRegistry(DataRegistry* handler);

	private:

		RegistryId getNewId() { return m_newId++; }
		RegistryId m_newId=1;

		struct Entry {
			std::string name;
			RegistryId id;
			DataRegistry* registry;
		};
		
		// fast access to entries
		// the int value refers to index of m_entries
		// these maps are filled when handlers are registered
		// maps are not serialized
		std::unordered_map<std::string, int> m_nameEntries;
		std::unordered_map<RegistryId, int> m_integerEntries;

		// this will be serialized
		std::vector<Entry> m_entries;
		
	};
}