#include "ProUnk/DataRegistries/MasterRegistry.h"

namespace prounk {

	DataRegistry* MasterRegistry::getRegistry(RegistryId id) {
		auto find = m_integerEntries.find(id);
		if (find != m_integerEntries.end()) {
			Entry& entry = m_entries[find->second];
			return entry.registry;
		}
		return nullptr;
	}
	DataRegistry* MasterRegistry::getRegistry(const std::string& name) {
		auto find = m_nameEntries.find(name);
		if (find != m_nameEntries.end()) {
			Entry& entry = m_entries[find->second];
			return entry.registry;
		}
		return nullptr;
	}

	RegistryId MasterRegistry::registerRegistry(DataRegistry* registry) {
		using namespace engone;
		if (!registry) {
			log::out << log::RED << "MasterRegistry::registerHandler - handler cannot be nullptr\n";
			return 0;
		}

		// find entry
		int entryIndex=-1;
		for (int i = 0; i < m_entries.size();i++) {
			Entry& entry = m_entries[i];
			if (entry.name == registry->getName()) {
				entryIndex = i;
				break;
			}
		}

		if (entryIndex == -1) {
			// entry does not exist, we are registering a new handler which happens if you made new world
			RegistryId registryId = getNewId();
			m_entries.push_back({ registry->getName(), registryId, registry });
			registry->m_id = registryId;
			
			log::out << "Registering new handler '"<< registry->getName() << "' as "<<registryId<<"\n";

			m_nameEntries[registry->getName()] = m_entries.size()-1;
			m_integerEntries[registry->getId()] = m_entries.size()-1;

			return registryId;
		}
		else {
			// entry exists
			Entry& entry = m_entries[entryIndex];
			if (!entry.registry) {
				// handler has not been registered but has existed before serialization
				entry.registry = registry;
				registry->m_id = entry.id;

				log::out << "Registering handler '" << registry->getName() << "' as " << entry.id << "\n";


				m_nameEntries[registry->getName()] = m_entries.size() - 1;
				m_integerEntries[registry->getId()] = m_entries.size() - 1;

				return entry.id;
			}
			else {
				log::out << log::RED << "MasterRegistry::registerHandler - Handler " << registry->getName() << " is already registered!\n";
				return 0;
			}
		}
	}
	void MasterRegistry::serialize() {
		using namespace engone;
		FileWriter file("handlerRegistry.dat");
		if (!file) {
			log::out << log::RED << "HandlerRegistry file '" << file.getPath() << "' cannot be serialized\n";
			return;
		}
		file.writeOne(m_newId);
		int count = m_entries.size();
		file.writeOne(count);
		for (Entry& entry : m_entries) {
			file.write(entry.name);
			file.writeOne(entry.id);
		}

		log::out << "MasterRegistry serialized "<<count<<" entries\n";
	}
	void MasterRegistry::deserialize() {
		using namespace engone;
		FileReader file("handlerRegistry.dat");
		if (!file) {
			log::out << log::RED << "HandlerRegistry file '" << file.getPath() << "' cannot be deserialized\n";
			return;
		}
		file.readOne(m_newId);
		int count;
		file.readOne(count);
		m_entries.resize(count);
		for (int i = 0; i < count;i++) {
			file.read(m_entries[i].name);
			file.readOne(m_entries[i].id);
			m_entries[i].registry = nullptr;
		}
		log::out << "MasterRegistry loaded " << count << " entries\n";
	}
}