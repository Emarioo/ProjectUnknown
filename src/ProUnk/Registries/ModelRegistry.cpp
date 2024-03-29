#include "ProUnk/Registries/ModelRegistry.h"

#include "Engone/FileModule/FileModule.h"

#include "Engone/Window.h"

namespace prounk {

	engone::ModelAsset* ModelRegistry::getModel(ModelId id) {
		if (id < 1 || id > m_entries.size())
			return nullptr;
		return m_entries[id-1].model;
	}
	ModelId ModelRegistry::registerModel(const std::string& modelName) {
		using namespace engone;
		ModelAsset* model = engone::GetActiveWindow()->getStorage()->load<ModelAsset>(modelName);
		return registerModel(model);
	}
	ModelId ModelRegistry::registerModel(engone::ModelAsset* model) {
		using namespace engone;
		if (!model) {
			REGISTER_IF_LEVEL(REGISTRY_LEVEL_ERROR)
				log::out << log::RED << "ModelRegistry::registerModel - model was nullptr\n";
			return -1;
		}

		int id = -1;
		// find name of model
		for (int i = 0; i < m_entries.size();i++){
			Entry& entry = m_entries[i];
			if (entry.name == model->getLoadName()) {
				id = i+1;
				break;
			}
		}

		if (id == -1) {
			// not found, register new
			id = getNewId();

			m_entries.push_back({});
			m_entries.back().name = model->getLoadName();
			m_entries.back().model = model;

			REGISTER_IF_LEVEL(REGISTRY_LEVEL_ALL)
				log::out << log::LIME << "ModelRegistry: Registered '" << model->getLoadName() << "' as " << id << "\n";
			
		}
		else {
			// found
			Entry& entry = m_entries[id-1];
			if (entry.model) {
				REGISTER_IF_LEVEL(REGISTRY_LEVEL_ERROR)
					log::out <<log::RED<< "ModelRegistry: '" << model->getLoadName() << "' is already registered\n";
			} else {
				entry.model = model; // the dataIndex existed before but it didn't have the model
				REGISTER_IF_LEVEL(REGISTRY_LEVEL_ALL)
					log::out << log::LIME << "ModelRegistry: (soft) Registered '" << model->getLoadName() << "' as " << id << "\n";
			}
		}
		return id;
	}
	void ModelRegistry::serialize() {
		using namespace engone;

		FileWriter file("ModelRegistry.dat");
		if (!file)
			return;
		int size = m_entries.size();
		file.write(&size);
		for (int i = 0; i < size;i++) {
			Entry& entry = m_entries[i];
			file.write(&entry.name);
		}
	}
	void ModelRegistry::deserialize() {
		using namespace engone;

		FileReader file("ModelRegistry.dat");
		if (!file)
			return;
		int size;
		file.read(&size);
		for (int i = 0; i < size; i++) {
			m_entries.push_back({});
			file.read(&m_entries.back().name);
		}
	}

}