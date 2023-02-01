#pragma once

#include "ProUnk/Registries/MasterRegistry.h"

#include "ProUnk/Combat/CombatData.h"
#include "ProUnk/Registries/InventoryRegistry.h"

#include "Engone/Utilities/FrameArray.h"

namespace prounk {

	struct ObjectCreatureInfo {
		std::string displayName;
		CombatData combatData;
		uint32 inventoryDataIndex = 0;
		int onGround = 0;
	};
	struct ObjectItemInfo {
		Item item;
	};
	struct ObjectWeaponInfo {
		CombatData combatData;
	};
	struct ObjectTriggerInfo {
		std::vector<engone::UUID> collisions;
		bool hit(engone::UUID uuid);
	};
	ObjectTriggerInfo& GetTriggerInfo(engone::EngineObject* object);
	
	// This class has a lot of repetition in it.
	// Can it be improved? macros perhaps?
	class ObjectInfoRegistry : public Registry {
	public:
		ObjectInfoRegistry() : Registry("object_info_registry") {}

		void serialize();
		void deserialize();

		ObjectItemInfo& getItemInfo(uint32 dataIndex);
		uint32 registerItemInfo();
		void unregisterItemInfo(uint32 dataIndex);
		
		ObjectWeaponInfo& getWeaponInfo(uint32 dataIndex);
		uint32 registerWeaponInfo();
		void unregisterWeaponInfo(uint32 dataIndex);

		ObjectCreatureInfo& getCreatureInfo(uint32 dataIndex);
		uint32 registerCreatureInfo(const std::string& name);
		void unregisterCreatureInfo(uint32 dataIndex);
		
		ObjectTriggerInfo& getTriggerInfo(uint32 dataIndex);
		uint32 registerTriggerInfo();
		void unregisterTriggerInfo(uint32 dataIndex);

	private:
		// Each allocated frame has 128 infos
		engone::FrameArray<ObjectCreatureInfo> m_creatureInfos{128};
		engone::FrameArray<ObjectItemInfo> m_itemInfos{128};
		engone::FrameArray<ObjectWeaponInfo> m_weaponInfos{128};
		engone::FrameArray<ObjectTriggerInfo> m_triggerInfos{128};
		
	};
}