#include "Engone/Registries/MasterRegistry.h"

namespace engone {

#define IF_LEVEL(level) if (HasLogLevel(REGISTRY_LEVEL, level))

	Registry::Registry(const std::string& name) : m_name(name) {}
	Registry::Registry(const std::string& name, const std::string& path) : m_name(name), m_path(path) {}
	const std::string& Registry::getName() { return m_name; };
	RegistryId Registry::getId() { return m_id; };
	void Registry::setPath(const std::string& path) {
		m_path = path;
	}
	const std::string& Registry::getPath() {
		return m_path;
	}
	Registry* MasterRegistry::getRegistry(RegistryId id) {
		return m_entries[id].registry;
	}
	Registry* MasterRegistry::getRegistry(const std::string& name) {
		auto find = m_nameEntries.find(name);
		if (find != m_nameEntries.end()) {
			Entry& entry = m_entries[find->second];
			return entry.registry;
		}
		return nullptr;
	}
	RegistryId MasterRegistry::registerRegistry(Registry* registry) {
		using namespace engone;
		if (!registry) {
			IF_LEVEL(REGISTRY_LEVEL_ERROR)
				log::out << log::RED << "MasterRegistry : Registry cannot be nullptr\n";
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
			RegistryId registryId = m_entries.size()+1;
			m_entries.push_back({ registry->getName(), registryId, registry });
			registry->m_id = registryId;
			
			m_nameEntries[registry->getName()] = registryId-1;

			IF_LEVEL(REGISTRY_LEVEL_INFO)
				log::out << "MasterRegistry : Registered '" << registry->getName() << "' as " << registryId << "\n";

			return registryId;
		} else {
			// entry exists
			Entry& entry = m_entries[entryIndex];
			if (!entry.registry) {
				// registry existed in last session but isn't registered now
				entry.registry = registry;
				registry->m_id = entry.id;

				IF_LEVEL(REGISTRY_LEVEL_INFO)
					log::out << "MasterRegistry : Registered '" << registry->getName() << "' as " << entry.id << "\n";

				m_nameEntries[registry->getName()] = entry.id-1;

				return entry.id;
			} else {
				if (entry.registry == registry)
					return entry.id;

				IF_LEVEL(REGISTRY_LEVEL_WARNING)
					log::out << log::YELLOW << "MasterRegistry : A registry is already registered as '" << registry->getName() << "'\n";
				return 0;
			}
		}
	}
	void MasterRegistry::serialize() {
		using namespace engone;
		FileWriter file(m_path);
		if (!file) {
			IF_LEVEL(REGISTRY_LEVEL_ERROR)
				log::out << log::RED << "MasterRegistry : Cannot serialize from path '" << file.getPath() << "'\n";
			return;
		}
		int count = m_entries.size();
		file.writeOne(count);
		for (Entry& entry : m_entries) {
			file.write(entry.name);
			file.writeOne(entry.id);
		}

		IF_LEVEL(REGISTRY_LEVEL_INFO)
			log::out << "MasterRegistry : serialized "<<count<<" entries\n";
	}
	void MasterRegistry::deserialize() {
		using namespace engone;
		FileReader file(m_path);
		if (!file) {
			IF_LEVEL(REGISTRY_LEVEL_ERROR)
				log::out << log::RED << "MasterRegistry : Cannot deserialize from path '" << file.getPath() << "'\n";
			return;
		}
		int count;
		file.readOne(count);
		m_entries.resize(count);
		for (int i = 0; i < count;i++) {
			file.read(m_entries[i].name);
			file.readOne(m_entries[i].id);
			m_entries[i].registry = nullptr;
		}
		IF_LEVEL(REGISTRY_LEVEL_INFO)
			log::out << "MasterRegistry : deserialized " << count << " entries\n";
	}
	void MasterRegistry::setPath(const std::string& path) {
		m_path = path;
	}
	const std::string& MasterRegistry::getPath() {
		return m_path;
	}
}