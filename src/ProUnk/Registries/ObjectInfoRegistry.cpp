#include "ProUnk/Registries/ObjectInfoRegistry.h"

// #include "ProUnk/GameApp.h"

#include "ProUnk/World/Session.h"

namespace prounk {

	bool ObjectTriggerInfo::hit(engone::UUID uuid) {
		for (engone::UUID& u : collisions) {
			if (u == uuid)
				return true;
		}
		return false;
	}

	ObjectTriggerInfo* GetTriggerInfo(engone::EngineObject* object) {
		return ((Dimension*)object->getWorld()->getUserData())->getParent()->objectInfoRegistry.getTriggerInfo(object->getObjectInfo());
	}

	void ObjectInfoRegistry::serialize() {
		using namespace engone;
		log::out << log::RED<<"ObjectInfoRegistry - Serialization is not implemented\n";
	}
	void ObjectInfoRegistry::deserialize() {
		using namespace engone;
		log::out << log::RED << "ObjectInfoRegistry - Deserialization is not implemented\n";
	}

	u32 ObjectInfoRegistry::registerItemInfo(ObjectItemInfo** outPtr) {
		ObjectItemInfo tmp{};
		return m_itemInfos.add(&tmp,(void**)outPtr) + 1;
	}
	ObjectItemInfo* ObjectInfoRegistry::getItemInfo(u32 dataIndex) {
		return (ObjectItemInfo*)m_itemInfos.get(dataIndex-1);
	}
	void ObjectInfoRegistry::unregisterItemInfo(u32 dataIndex) {
		// Todo: If ComplexData is handled with registries then it needs to be unregistered here.
		//		ComplexData is a member of Item so it is fine right now (2023-0-19).
		m_itemInfos.remove(dataIndex - 1);
	}

	u32 ObjectInfoRegistry::registerWeaponInfo(ObjectWeaponInfo** outPtr) {
		ObjectWeaponInfo tmp{};
		return m_weaponInfos.add(&tmp,(void**)outPtr) + 1;
	}
	ObjectWeaponInfo* ObjectInfoRegistry::getWeaponInfo(u32 dataIndex) {
		return (ObjectWeaponInfo*)m_weaponInfos.get(dataIndex - 1);
	}
	void ObjectInfoRegistry::unregisterWeaponInfo(u32 dataIndex) {
		m_weaponInfos.remove(dataIndex-1);
	}

	u32 ObjectInfoRegistry::registerCreatureInfo(const std::string& name, ObjectCreatureInfo** outPtr) {
		ObjectCreatureInfo tmp{ name };
		return m_creatureInfos.add(&tmp, (void**)outPtr) + 1;
	}
	ObjectCreatureInfo* ObjectInfoRegistry::getCreatureInfo(u32 dataIndex) {
		return (ObjectCreatureInfo*)m_creatureInfos.get(dataIndex - 1);
	}
	void ObjectInfoRegistry::unregisterCreatureInfo(u32 dataIndex) {
		// Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
		auto oinfo = (ObjectCreatureInfo*)m_creatureInfos.get(dataIndex-1);
		// if (oinfo->inventoryDataIndex)
			// session->inventoryRegistry.destroyInventory(oinfo->inventoryDataIndex);

		m_creatureInfos.remove(dataIndex-1);
	}

	u32 ObjectInfoRegistry::registerTriggerInfo(ObjectTriggerInfo** outPtr) {
		ObjectTriggerInfo tmp{};
		return m_triggerInfos.add(&tmp, (void**)outPtr) + 1;
	}
	ObjectTriggerInfo* ObjectInfoRegistry::getTriggerInfo(u32 dataIndex) {
		return (ObjectTriggerInfo*)m_triggerInfos.get(dataIndex - 1);
	}
	void ObjectInfoRegistry::unregisterTriggerInfo(u32 dataIndex) {
		// Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
		auto oinfo = m_triggerInfos.get(dataIndex - 1);
		//if (oinfo->inventoryDataIndex)
		//	session->inventoryRegistry.destroyInventory(oinfo->inventoryDataIndex);

		m_triggerInfos.remove(dataIndex - 1);
	}
}