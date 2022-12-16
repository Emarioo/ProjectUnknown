#pragma once

#include "ProUnk/DataRegistries/MasterRegistry.h"

#include "Engone/AssetModule.h"

namespace prounk {

	typedef int ModelId;
	class ModelRegistry : public DataRegistry {
	public:
		ModelRegistry() : DataRegistry("model_registry") {}

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
		ModelId m_newId = 1; // id 0 is seen as invalid

		std::vector<Entry> m_entries;

	};
}