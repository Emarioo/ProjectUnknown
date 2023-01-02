#include "ProUnk/Registries/ModelRegistry.h"

#include "Engone/Utilities/FileUtility.h"

namespace prounk {

	engone::ModelAsset* ModelRegistry::getModel(ModelId id) {
		if (id < 1 || id > m_entries.size())
			return nullptr;
		return m_entries[id-1].model;
	}
	ModelId ModelRegistry::registerModel(engone::ModelAsset* model) {
		using namespace engone;
		if (!model) {
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

			log::out << "Registered NEW model '" << model->getLoadName() << "' as " << id << "\n";
			
		}
		else {
			// found
			Entry& entry = m_entries[id-1];
			if (entry.model) {
				log::out <<log::RED<< "Model '" << model->getLoadName() << "' already registered\n";
			} else {
				entry.model = model;
				log::out << "Registered model '" << model->getLoadName() << "' as " << id << "\n";
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
			file.write(entry.name);
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
			file.read(m_entries.back().name);
		}
	}

}