#pragma once

#include "ProUnk/Registries/MasterRegistry.h"

#include "Engone/AssetModule.h"

namespace prounk {

	typedef int ModelId;
	class ModelRegistry : public Registry {
	public:
		ModelRegistry() : Registry("model_registry") {}

		void serialize();
		void deserialize();

		// may return nullptr
		// id should not be out of bounds (crash otherwise)
		engone::ModelAsset* getModel(ModelId id);

		ModelId registerModel(const std::string& modelName);
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