#pragma once

#include "ProUnk/DataHandlers/DataHandlerRegistry.h"

#include "Engone/AssetModule.h"

namespace prounk {

	typedef int ModelId;
	class ModelHandler : public DataHandler {
	public:
		ModelHandler() : DataHandler("model_handler") {}

		void serialize();
		void deserialize();

		// may return nullptr
		// id should not be out of bounds (crash otherwise)
		engone::ModelAsset* getModel(ModelId id);

		// don't register models until deserialization is done
		ModelId registerModel(engone::ModelAsset* model);

	private:
		struct Entry {
			std::string name;
			engone::ModelAsset* model;
		};

		ModelId getNewId() { return m_newId++; }
		ModelId m_newId = 0; // id -1 is seen as invalid

		std::vector<Entry> m_entries;

	};
}