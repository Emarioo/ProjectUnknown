#pragma once

#include "Engone/Logger.h"
#include "Engone/Utilities/FileUtility.h"

namespace prounk {

#define REGISTRY_LEVEL_ALL 0xffffffff
#define REGISTRY_LEVEL_INFO 0x01
#define REGISTRY_LEVEL_WARNING 0x02
#define REGISTRY_LEVEL_ERROR 0x04
#define REGISTRY_LEVEL 991528

#define REGISTER_IF_LEVEL(LEVEL) if (HasLogLevel(REGISTRY_LEVEL, LEVEL))

	typedef uint32_t RegistryId;
	class Registry {
	public:
		Registry(const std::string& name);
		Registry(const std::string& name, const std::string& path);

		virtual void serialize() {}
		virtual void deserialize() {}

		const std::string& getName();
		RegistryId getId();

		void setPath(const std::string& path);
		const std::string& getPath();

	private:
		RegistryId m_id=0;
		std::string m_name="Unnamed";
		std::string m_path="Unnamed.dat";

		friend class MasterRegistry;
	};
	/*
		registry id 0 is seen as null.
	*/
	class MasterRegistry {
	public:
		MasterRegistry() = default;

		void serialize();
		void deserialize();

		// returns nullptr if id is 0 or if registry hasn't been registered
		Registry* getRegistry(RegistryId id);
		// returns nullptr if name doesn't exist or if registry hasn't been registered
		Registry* getRegistry(const std::string& name);

		// returns the generate id for the registry. If the registry's name already exists then 0 is returned
		// to indicate that the name is already used or that you tried to register the registry twice.
		RegistryId registerRegistry(Registry* registry);

		// path where registry will be serialized
		// make sure to set this before serializing
		// an example of path would be "data/master_registry.dat"
		void setPath(const std::string& path);
		const std::string& getPath();

	private:
		struct Entry {
			std::string name;
			RegistryId id;
			Registry* registry;
		};
		// this will be serialized
		std::vector<Entry> m_entries;
		// make sure the path is what you want when serializing
		std::string m_path = "master_registry.dat";

		// fast access to entries
		std::unordered_map<std::string, RegistryId> m_nameEntries;
	};
}