#pragma once

#include "ProUnk/Registries/MasterRegistry.h"

#include "ProUnk/Combat/CombatData.h"
#include "ProUnk/Registries/InventoryRegistry.h"

namespace prounk {

	struct ObjectCreatureInfo {
		//CombatData* combatData=nullptr;
		CombatData combatData;
		int inventoryId=0;
	};
	struct ObjectItemInfo {
		Item item;
	};
	struct ObjectWeaponInfo {
		//CombatData* combatData = nullptr;
		CombatData combatData;
	};

	// This class has a lot of repetition in it.
	// Can it be improved? macros perhaps?
	class ObjectInfoRegistry : public Registry {
	public:
		ObjectInfoRegistry() : Registry("object_info_registry","object_info_registry.dat") {}

		void serialize();
		void deserialize();

		ObjectItemInfo& getItemInfo(int id);
		int registerItemInfo();
		
		ObjectCreatureInfo& getCreatureInfo(int id);
		int registerCreatureInfo();
		
		ObjectWeaponInfo& getWeaponInfo(int id);
		int registerWeaponInfo();

	private:
		// this will be serialized
		std::vector<ObjectCreatureInfo> m_creatureInfos;
		std::vector<ObjectItemInfo> m_itemInfos;
		std::vector<ObjectWeaponInfo> m_weaponInfos;
	};
}