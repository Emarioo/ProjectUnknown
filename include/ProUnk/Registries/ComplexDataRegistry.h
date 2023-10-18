#pragma once

#include "ProUnk/Registries/MasterRegistry.h"
#include "Engone/Structures/HashMap.h"
#include "Engone/Structures/StableArray.h"
#include "Engone/Structures/Array.h"

namespace prounk {
	struct ComplexPropertyType {
		uint32_t dataIndex;
		std::string name;
		// color?
	};
	class ComplexData {
	public:
		ComplexData() : m_properties(sizeof(Entry),ALLOC_TYPE_HEAP) {}

		bool set(uint32_t propertyDataIndex, float value);
		bool set(ComplexPropertyType* complexProperty, float value);

		float get(uint32_t propertyDataIndex);
		float get(ComplexPropertyType* complexProperty);

		bool sameAs(ComplexData* data);

		// Todo: Quick use of function?
		//float& operator[](int propertyDataIndex);

		// Returns nullpt when allocation fails.
		bool copy(ComplexData* complexData);

		uint32_t getDataIndex();

		//engone::HashMap& getMap();
		struct Entry {
			uint32_t prop;
			float value;
		};
		engone::Array& getList();

	private:
		uint32_t m_dataIndex=0;
		// int is dataIndex to ComplexPropertyType
		// uint32_t is raw data. Could be int, float or 4 chars.
		//engone::HashMap m_map{}; // for uint32_t, works for floats too with some magic

		//std::vector<Entry> m_properties;
		engone::Array m_properties;

		friend class ComplexDataRegistry;
	};

	class ComplexDataRegistry : public Registry {
	public:
		ComplexDataRegistry() : Registry("complex_data_registry") {}
	
		// returns 0 if a property with the same name exists.
		ComplexPropertyType* registerProperty(const std::string& name);
		void unregisterProperty(const std::string& name);
		// returns 0 if not found.
		ComplexPropertyType* getProperty(const std::string& name);
		// returns empty string if invalid dataIndex.
		ComplexPropertyType* getProperty(uint32_t dataIndex);

		// Creates ComplexInfo which you can use
		// note that the pointer can become invalid if you register other data while using the pointer.
		// this is due to resizing of memory.
		ComplexData* registerData();
		void unregisterData(uint32_t dataIndex);
		// returns nullptr if dataIndex was invalid
		ComplexData* getData(uint32_t dataIndex);
	
		// Todo: Implement serialize and deserialize

	private:
		std::unordered_map<std::string, uint32_t> m_propertyStrings;

		engone::StableArray<ComplexPropertyType> m_propertyTypes;
		engone::StableArray<ComplexData> m_complexDatas;

		//std::vector<ComplexPropertyType> m_propertyTypes;
		//std::vector<ComplexData> m_complexDatas;
	};
}