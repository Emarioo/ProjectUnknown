#pragma once

//#include "ItemType.h"

#include "Engone/Assets/ModelAsset.h"

namespace prounk {
	class Item {
	public:
		Item() = default;
		//Item(const std::string& name);
		//Item(const std::string& name, int count);

		const std::string& getName() { return m_name; }
		void setName(const std::string& name) { m_name=name; }

		int getCount() { return m_count; }
		void setCount(int count) { m_count = count; }

		engone::ModelAsset* getModel() { return m_model; }
		void setModel(engone::ModelAsset* model) { m_model = model; }
	
	private:
		int m_count = 0;
		std::string m_name;
		engone::ModelAsset* m_model;

		int m_handlerId=0;
		int m_dataIndex;

	};
}