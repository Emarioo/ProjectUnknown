#include "ProUnk/DataHandlers/ModelHandler.h"

#include "Engone/Utilities/FileUtility.h"

namespace prounk {

	engone::ModelAsset* ModelHandler::getModel(ModelId id) {
		return m_entries[id].model;
	}
	ModelId ModelHandler::registerModel(engone::ModelAsset* model) {
		using namespace engone;
		if (!model) {
			log::out << log::RED << "ModelHandler::registerModel - model was nullptr\n";
			return -1;
		}

		int id = -1;
		// find name of model
		for (int i = 0; i < m_entries.size();i++){
			Entry& entry = m_entries[i];
			if (entry.name == model->getLoadName()) {
				id = i;
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
			
			return id;
		}
		else {
			// found
			Entry& entry = m_entries[id];
			if (entry.model) {
				log::out <<log::RED<< "Model '" << model->getLoadName() << "' already registered\n";
			}
			else {
				entry.model = model;
				log::out << "Registered model '" << model->getLoadName() << "' as " << id << "\n";
				return id;
			}
		}
		return -1;
	}
	void ModelHandler::serialize() {
		using namespace engone;

		FileWriter file("modelHandler.dat");

		int size = m_entries.size();
		file.write(&size);
		for (int i = 0; i < size;i++) {
			Entry& entry = m_entries[i];
			file.write(entry.name);
		}
	}
	void ModelHandler::deserialize() {
		using namespace engone;

		FileReader file("modelHandler.dat");

		int size;
		file.read(&size);
		for (int i = 0; i < size; i++) {
			m_entries.push_back({});
			file.read(m_entries.back().name);
		}
	}

}