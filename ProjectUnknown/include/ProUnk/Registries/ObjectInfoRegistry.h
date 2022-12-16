#pragma once

#include "Engone/Registries/MasterRegistry.h"

//#include "Engone/Assets/Animator.h"
#include "ProUnk/Combat/CombatData.h"
#include "ProUnk/Registries/InventoryRegistry.h"

namespace prounk {

	struct ObjectPlayerInfo {
		CombatData* combatData=nullptr;
		int inventoryId=0;
		engone::Animator animator;
	};
	struct ObjectItemInfo {
		Item item;
	};

	class ObjectInfoRegistry : public engone::Registry {
	public:
		ObjectInfoRegistry() : Registry("object_info_registry","object_info_registry.dat") {}

		void serialize();
		void deserialize();

		ObjectItemInfo& getItemInfo(int id);
		ObjectPlayerInfo& getPlayerInfo(int id);

		int registerItemInfo();
		int registerPlayerInfo();

	private:
		// this will be serialized
		std::vector<ObjectPlayerInfo> m_playerInfos;
		std::vector<ObjectItemInfo> m_itemInfos;
	};
}