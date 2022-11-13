#pragma once

#include "Engone/Logger.h"

namespace prounk {
	typedef int HandlerId;

	class DataHandler {
	public:
		DataHandler(const char* name) : m_name(name) {}

		virtual void serialize() = 0;
		virtual void deserialize() = 0;

		const char* getName() { return m_name; };
		HandlerId getId() { return m_id; };

	private:

		HandlerId m_id;
		const char* m_name;

		friend class DataHandlerRegistry;
	};
	/*
		handler id 0 is seen as nullptr.
	*/
	class DataHandlerRegistry {
	public:
		DataHandlerRegistry() = default;

		void serialize();
		void deserialize();

		// returns nullptr if id doesn't exist or if handler hasn't been registered
		DataHandler* getHandler(HandlerId id);
		// returns nullptr if name doesn't exist or if handler hasn't been registered
		DataHandler* getHandler(const std::string& name);

		HandlerId registerHandler(DataHandler* handler);

	private:

		HandlerId getNewId() { return m_newId++; }
		HandlerId m_newId=1;

		struct Entry {
			std::string name;
			HandlerId id;
			DataHandler* handler;
		};
		
		// fast access to entries
		// the int value refers to index of m_entries
		// these maps are filled when handlers are registered
		// maps are not serialized
		std::unordered_map<std::string, int> m_nameEntries;
		std::unordered_map<HandlerId, int> m_integerEntries;

		// this will be serialized
		std::vector<Entry> m_entries;
		
	};
}