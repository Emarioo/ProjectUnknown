#pragma once

#include "ProUnk/Registries/MasterRegistry.h"
#include "Engone/Utilities/HashMap.h"
#include "Engone/Utilities/StableArray.h"

namespace prounk {
	struct ComplexPropertyType {
		uint32_t dataIndex;
		std::string name;
		// color?
	};
	class ComplexData {
	public:
		ComplexData();
		typedef float T;
		//template<class T>
		bool set(int propertyDataIndex, T value) {
			if (sizeof(T) != sizeof(uint32_t)) {
				return false;
			}
			return m_map.insert(propertyDataIndex, *(engone::HashMap::Value*)&value);
		}
		//template<class T>
		bool set(ComplexPropertyType* complexProperty, T value) {
			if (sizeof(T) != sizeof(uint32_t)) {
				return false;
			}
			return m_map.insert(complexProperty->dataIndex, *(engone::HashMap::Value*)&value);
		}
		//template<class T>
		T get(uint32_t propertyDataIndex) {
			engone::HashMap::Value value = m_map.get(propertyDataIndex);
			return *(T*)&value;
		}
		//template<class T>
		T get(ComplexPropertyType* complexProperty) {
			engone::HashMap::Value value = m_map.get(complexProperty->dataIndex);
			return *(T*)&value;
		}

		bool sameAs(ComplexData* data);
		// Todo: Quick use of function?
		//float& operator[](int propertyDataIndex);

		// can fail if allocation failed
		ComplexData* copy();

		uint32_t getDataIndex();

		engone::HashMap& getMap();

	private:
		uint32_t m_dataIndex=0;
		// int is dataIndex to ComplexPropertyType
		// uint32_t is raw data. Could be int, float or 4 chars.
		//std::unordered_map<int, uint32_t> data32;
		engone::HashMap m_map{}; // for uint32_t, works for floats too with some magic

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