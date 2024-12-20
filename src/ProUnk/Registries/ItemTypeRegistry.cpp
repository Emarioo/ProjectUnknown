
#include "ProUnk/Registries/ItemTypeRegistry.h"

#include "Engone/FileModule/FileModule.h"

namespace prounk {

	void ItemTypeRegistry::serialize() {
		using namespace engone;
		FileWriter file("itemTypeRegistry.dat");
		if (!file) {
			log::out << log::RED << "ItemTypeRegistry file '" << file.getPath() << "' cannot be serialized\n";
			return;
		}
		// file.writeOne(m_newType);
		int count = m_itemTypes.size();
		
		// file.writeOne(count);
		file.write(&count);
		for (ItemTypeInfo& entry : m_itemTypes) {
			file.write(&entry.itemType);
			file.write(&entry.name);
		}

		log::out << "ItemTypeRegistry serialized " << count << " entries\n";
	}
	void ItemTypeRegistry::deserialize() {
		using namespace engone;
		FileReader file("itemTypeRegistry.dat");
		if (!file) {
			log::out << log::RED << "ItemTypeRegistry file '" << file.getPath() << "' cannot be deserialized\n";
			return;
		}
		//file.readOne(m_newType);
		int count;
		file.read(&count);
		m_itemTypes.resize(count);
		for (int i = 0; i < count; i++) {
			ItemTypeInfo& entry = m_itemTypes[i];
			file.read(&entry.itemType);
			file.read(&entry.name);
		}
		log::out << "ItemTypeRegistry loaded " << count << " entries\n";
	}
	const ItemTypeInfo* ItemTypeRegistry::getType(ItemType type) {
		if (type < 1 || type > m_itemTypes.size())
			return nullptr;
		return &m_itemTypes[type - 1]; // <- visual studio complains about some wider type and overflow
	}
	const ItemTypeInfo* ItemTypeRegistry::getType(const std::string& name) {
		auto find = m_nameTypes.find(name);
		if (find != m_nameTypes.end()) {
			ItemTypeInfo& entry = m_itemTypes[find->second-1];
			return &entry;
		}
		return nullptr;
	}
	bool IsNameValid(const char* name) {
		while (*name!='\0') {
			if ((*name) != '_' &&
				((*name) < 'a' || (*name) > 'z'))
				return false;
			name++;
		}
		return true;

	}
	// takes registered name and turns it into display name
	static std::string GenerateDisplayName(const std::string& name) {
		std::string out;
		out.reserve(name.length());
		bool capital = true;
		for (int i = 0; i < name.length();i++) {
			char chr = name[i];
			if (chr == '_') {
				out += ' ';
				capital = true;
			}else if(capital) {
				capital = false;
				out += (chr - 'a' + 'A'); // lower case is assumed
			} else {
				out += chr;
			}
		}
		return out;
	}
	ItemType ItemTypeRegistry::registerType(const std::string& name, ModelId modelId) {
		return registerType(name, modelId, GenerateDisplayName(name));
	}
	ItemType ItemTypeRegistry::registerType(const std::string& name, ModelId modelId, const std::string& displayName) {
		using namespace engone;
		if (name.empty()) {
			log::out << log::RED << "ItemTypeRegistry: Cannot register '"<<name<<"' (empty string)\n";
			return 0;
		}
		if (!IsNameValid(name.c_str())) {
			log::out << log::RED << "ItemTypeRegistry: Cannot register '" << name << "' (only 'a' to 'z' and '_' are allowed)\n";
			return 0;
		}
		if (!modelId) {
			log::out << log::RED <<"ItemTypeRegistry: Cannot register '" << name << "' with 0 as modelId\n";
			return 0;
		}
		int itemType = 0;
		// find name of model
		for (int i = 0; i < m_itemTypes.size(); i++) {
			ItemTypeInfo& entry = m_itemTypes[i];
			if (entry.name == name) {
				itemType = i + 1;
				break;
			}
		}

		if (itemType == 0) {
			// not found, register new
			itemType = m_itemTypes.size()+1;

			m_itemTypes.push_back({});
			m_itemTypes.back().itemType = itemType;
			m_itemTypes.back().name = name;
			m_itemTypes.back().displayName = displayName;
			m_itemTypes.back().modelId = modelId;
			
			m_nameTypes[name] = itemType;

			REGISTER_IF_LEVEL(REGISTRY_LEVEL_ALL)
			log::out << log::LIME<<"ItemTypeRegistry: Registered '" << name << "' as " << itemType << "\n";

		} else {
			// found, the info is already there.
			// you may want to load more info into ItemTypeInfo.
			REGISTER_IF_LEVEL(REGISTRY_LEVEL_ERROR)
			log::out << log::RED << "ItemTypeRegistry: '" << name << "' is already registered\n";
			m_nameTypes[name] = itemType;
		}
		return itemType;
	}
}