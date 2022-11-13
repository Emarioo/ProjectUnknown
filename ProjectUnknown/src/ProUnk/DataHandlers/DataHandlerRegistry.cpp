#include "ProUnk/DataHandlers/DataHandlerRegistry.h"

#include "Engone/Utilities/FileUtility.h"

namespace prounk {

	DataHandler* DataHandlerRegistry::getHandler(HandlerId id) {
		auto find = m_integerEntries.find(id);
		if (find != m_integerEntries.end()) {
			Entry& entry = m_entries[find->second];
			return entry.handler;
		}
		return nullptr;
	}
	DataHandler* DataHandlerRegistry::getHandler(const std::string& name) {
		auto find = m_nameEntries.find(name);
		if (find != m_nameEntries.end()) {
			Entry& entry = m_entries[find->second];
			return entry.handler;
		}
		return nullptr;
	}

	HandlerId DataHandlerRegistry::registerHandler(DataHandler* handler) {
		using namespace engone;
		if (!handler) {
			log::out << log::RED << "DataHandlerRegistry::registerHandler - handler cannot be nullptr\n";
			return 0;
		}

		// find entry
		int entryIndex=-1;
		for (int i = 0; i < m_entries.size();i++) {
			Entry& entry = m_entries[i];
			if (entry.name == handler->getName()) {
				entryIndex = i;
				break;
			}
		}

		if (entryIndex == -1) {
			// entry does not exist, we are registering a new handler which happens if you made new world
			HandlerId handlerId = getNewId();
			m_entries.push_back({ handler->getName(),handlerId, handler });
			handler->m_id = handlerId;
			
			log::out << "Registering new handler '"<<handler->getName() << "' as "<<handlerId<<"\n";

			m_nameEntries[handler->getName()] = m_entries.size()-1;
			m_integerEntries[handler->getId()] = m_entries.size()-1;

			return handlerId;
		}
		else {
			// entry exists
			Entry& entry = m_entries[entryIndex];
			if (!entry.handler) {
				// handler has not been registered but has existed before serialization
				entry.handler = handler;
				handler->m_id = entry.id;

				log::out << "Registering handler '" << handler->getName() << "' as " << entry.id << "\n";


				m_nameEntries[handler->getName()] = m_entries.size() - 1;
				m_integerEntries[handler->getId()] = m_entries.size() - 1;

				return entry.id;
			}
			else {
				log::out << log::RED << "DataHandlerRegistry::registerHandler - Handler " << handler->getName() << " is already registered!\n";
				return 0;
			}
		}
	}
	void DataHandlerRegistry::serialize() {
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

		log::out << "DataHandlerRegistry serialized "<<count<<" entries\n";
	}
	void DataHandlerRegistry::deserialize() {
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
			m_entries[i].handler = nullptr;
		}
		log::out << "DataHandlerRegistry loaded " << count << " entries\n";
	}
}