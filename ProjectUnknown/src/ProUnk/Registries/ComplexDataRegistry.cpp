#include "ProUnk/Registries/ComplexDataRegistry.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	uint32_t ComplexData::getDataIndex() {
		return m_dataIndex;
	}
	bool ComplexData::set(uint32_t propertyDataIndex, float value) {
		for (int i = 0; i < m_properties.size(); i++) {
			Entry* entry = (Entry*)m_properties.get(i);
			if (entry->prop == propertyDataIndex) {
				entry->value = value;
				return true;
			} else if (entry->prop > propertyDataIndex) {
				// not found we do some rearranging
				Entry e;
				e.prop = propertyDataIndex;
				e.value = value;

				m_properties.insert(i, &e);
				return true;
			}
		}
		Entry e;
		e.prop = propertyDataIndex;
		e.value = value;
		m_properties.add(&e);
		//m_properties.push_back(e);
		return true;
		//return m_map.insert(propertyDataIndex, *(engone::HashMap::Value*)&value);
	}
	bool ComplexData::set(ComplexPropertyType* complexProperty, float value) {
		return set(complexProperty->dataIndex, value);
	}
	float ComplexData::get(uint32_t propertyDataIndex) {
		for (int i = 0; i < m_properties.size(); i++) {
			Entry* entry = (Entry*)m_properties.get(i);
			if (entry->prop == propertyDataIndex) {
				return entry->value;
			}
		}
		return 0;
		//engone::HashMap::Value value = m_map.get(propertyDataIndex);
		//return *(float*)&value;
	}
	float ComplexData::get(ComplexPropertyType* complexProperty) {
		return get(complexProperty->dataIndex);
	}
	bool ComplexData::sameAs(ComplexData* data) {
		if (data->m_properties.size() != m_properties.size())
			return false;

		int index = 0;
		for (int i = 0; i < m_properties.size();i++) {
			Entry* entry = (Entry*)m_properties.get(i);
			if (entry->prop != entry->prop ||
				entry->value != entry->value)
				return false;
		}
		return true;

		//return m_map.sameAs(data->getMap());
	}
	//engone::HashMap& ComplexData::getMap() {
	//	return m_map;
	//}
	engone::Array& ComplexData::getList() {
		return m_properties;
	}
	bool ComplexData::copy(ComplexData* complexData) {
		if (!complexData)
			return false;

		m_properties.copy(&complexData->m_properties);
		// dataIndex is unique and should not be copied
		 
		//newData->m_properties.resize(m_properties.size());
		//memcpy(newData->m_properties.data(), m_properties.data(), m_properties.size() * sizeof(Entry));
		
		return true;

		//Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
		//engone::HashMap temp;
		//bool yes = getMap().copy(temp);
		//if (!yes) {
		//	// Copy function will cleanup allocations during failure.
		//	// Otherwise the deconstructor of the hash map will clean it up.
		//	return nullptr;
		//}

		//ComplexData* newData = session->complexDataRegistry.registerData();
		//if (!newData) {
		//	return nullptr;
		//}
		//temp.preventDeconstructor(); // important that the temp hash map isn't freed.
		//newData->m_map = temp; // important that the hashmap doesn't have a copy constructor which messes things up
		//return newData;
	}
	//template<class T>
	//bool ComplexData::set(int propertyType, T value) {
	//	if (sizeof(T) != sizeof(uint32_t)) {
	//		return false;
	//	}
	//	return m_map.insert(propertyType, value);
	//}
	//template<class T>
	//T ComplexData::get(int propertyType) {
	//	engone::HashMap::Value value = m_map.get(propertyType);
	//	return *(T*)&value;
	//}
	//float& ComplexData::operator[](int key) {
	//	return *(float*)&m_map[key];
	//}

	ComplexPropertyType* ComplexDataRegistry::registerProperty(const std::string& name) {
		using namespace engone;
		auto find = m_propertyStrings.find(name);
		if (find != m_propertyStrings.end()) {
			// type with name already exists
			REGISTER_IF_LEVEL(REGISTRY_LEVEL_WARNING)
				log::out << log::YELLOW<<"ComplexDataRegistry: '" << name << "' is already registered (as '"<<find->second<<"')\n";

			return nullptr; // returns null because intention of registering a new property failed.
		}
		uint32_t dataIndex = m_propertyTypes.add({}) + 1;
		ComplexPropertyType* prop = m_propertyTypes.get(dataIndex-1);
		if (!prop)
			return nullptr;

		prop->dataIndex = dataIndex;
		prop->name = name;
		m_propertyStrings[name] = dataIndex;

		REGISTER_IF_LEVEL(REGISTRY_LEVEL_ALL)
			log::out << log::LIME << "ComplexDataRegistry: Registered '" << name << "' as " << dataIndex << "\n";

		return prop;
	}
	void ComplexDataRegistry::unregisterProperty(const std::string& name) {
		using namespace engone;
		auto find = m_propertyStrings.find(name);
		if (find == m_propertyStrings.end()) {
			REGISTER_IF_LEVEL(REGISTRY_LEVEL_WARNING)
				log::out << log::YELLOW << "ComplexDataRegistry: Cannot find and unregister '"<<name<<"'.\n";
			return;
		}
		m_propertyTypes.remove(find->second - 1);
	}
	ComplexPropertyType* ComplexDataRegistry::getProperty(const std::string& name) {
		auto find = m_propertyStrings.find(name);
		if (find == m_propertyStrings.end())
			return nullptr;
		return m_propertyTypes.get(find->second-1);
	}
	ComplexPropertyType* ComplexDataRegistry::getProperty(uint32_t dataIndex) {
		return m_propertyTypes.get(dataIndex - 1);
	}
	ComplexData* ComplexDataRegistry::registerData() {
		uint32_t dataIndex = m_complexDatas.add({})+1;
		ComplexData* data = m_complexDatas.get(dataIndex - 1);
		data->m_dataIndex = dataIndex;
		return data;
	}
	void ComplexDataRegistry::unregisterData(uint32_t dataIndex) {
		m_complexDatas.remove(dataIndex - 1);
	}
	ComplexData* ComplexDataRegistry::getData(uint32_t dataIndex) {
		return m_complexDatas.get(dataIndex-1);
	}
}