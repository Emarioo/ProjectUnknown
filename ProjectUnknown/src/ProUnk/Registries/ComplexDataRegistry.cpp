#include "ProUnk/Registries/ComplexDataRegistry.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	ComplexData::ComplexData() : m_map(20) {

	}
	uint32_t ComplexData::getDataIndex() {
		return m_dataIndex;
	}
	bool ComplexData::sameAs(ComplexData* data) {
		return m_map.sameAs(data->getMap());
	}
	engone::HashMap& ComplexData::getMap() {
		return m_map;
	}
	ComplexData* ComplexData::copy() {
		Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
		engone::HashMap temp;
		bool yes = getMap().copy(temp);
		if (!yes) {
			// Copy function will cleanup allocations during failure.
			// Otherwise the deconstructor of the hash map will clean it up.
			return nullptr;
		}

		ComplexData* newData = session->complexDataRegistry.registerData();
		if (!newData) {
			return nullptr;
		}
		temp.preventDeconstructor(); // important that the temp hash map isn't freed.
		newData->m_map = temp; // important that the hashmap doesn't have a copy constructor which messes things up
		return newData;
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
			log::out << log::YELLOW<<"Already registered property type '" << name << "' as " << find->second << "\n";

			return nullptr; // returns null because intention of registering a new property failed.
		}
		uint32_t dataIndex = m_propertyTypes.add({}) + 1;
		ComplexPropertyType* prop = m_propertyTypes.get(dataIndex-1);
		if (!prop)
			return nullptr;

		prop->dataIndex = dataIndex;
		prop->name = name;
		m_propertyStrings[name] = dataIndex;

		log::out << "Registered property type '" << name << "' as " << dataIndex << "\n";

		return prop;
	}
	void ComplexDataRegistry::unregisterProperty(const std::string& name) {
		using namespace engone;
		auto find = m_propertyStrings.find(name);
		if (find == m_propertyStrings.end()) {
			log::out << log::YELLOW << "Cannot find and unregister property type '"<<name<<"'.\n";
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