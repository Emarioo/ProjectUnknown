#pragma once

#include "ProUnk/Registries/MasterRegistry.h"

#include "ProUnk/Combat/CombatData.h"
#include "ProUnk/Registries/InventoryRegistry.h"

#include "Engone/Structures/FrameArray.h"

namespace prounk {

	struct ObjectCreatureInfo {
		std::string displayName;
		CombatData combatData;
		uint32 inventoryDataIndex = 0;
		int onGround = 0;
		engone::UUID heldObject=0;
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
	ObjectTriggerInfo* GetTriggerInfo(engone::EngineObject* object);
	
	// This class has a lot of repetition in it.
	// Can it be improved? macros perhaps?
	class ObjectInfoRegistry : public Registry {
	public:
		ObjectInfoRegistry() : Registry("object_info_registry") {}

		void serialize();
		void deserialize();

		// Todo: change reference to pointer

		ObjectItemInfo* getItemInfo(uint32 dataIndex);
		uint32 registerItemInfo(ObjectItemInfo** outPtr = nullptr);
		void unregisterItemInfo(uint32 dataIndex);
		
		ObjectWeaponInfo* getWeaponInfo(uint32 dataIndex);
		uint32 registerWeaponInfo(ObjectWeaponInfo** outPtr = nullptr);
		void unregisterWeaponInfo(uint32 dataIndex);

		ObjectCreatureInfo* getCreatureInfo(uint32 dataIndex);
		uint32 registerCreatureInfo(const std::string& name, ObjectCreatureInfo** outPtr=nullptr);
		void unregisterCreatureInfo(uint32 dataIndex);
		
		ObjectTriggerInfo* getTriggerInfo(uint32 dataIndex);
		uint32 registerTriggerInfo(ObjectTriggerInfo** outPtr = nullptr);
		void unregisterTriggerInfo(uint32 dataIndex);

	private:

#define USE_ALLOC_TYPE ALLOC_TYPE_GAME_MEMORY
		engone::FrameArray m_creatureInfos{ sizeof(ObjectCreatureInfo),32,USE_ALLOC_TYPE };
		engone::FrameArray m_itemInfos{ sizeof(ObjectItemInfo),32,USE_ALLOC_TYPE };
		engone::FrameArray m_weaponInfos{ sizeof(ObjectWeaponInfo),32,USE_ALLOC_TYPE };
		engone::FrameArray m_triggerInfos{ sizeof(ObjectTriggerInfo),32,USE_ALLOC_TYPE };
		
	};
}