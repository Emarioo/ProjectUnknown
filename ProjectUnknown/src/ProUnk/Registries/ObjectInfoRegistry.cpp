#include "ProUnk/Registries/ObjectInfoRegistry.h"

#include "ProUnk/GameApp.h"

namespace prounk {

	void ObjectInfoRegistry::serialize() {
		using namespace engone;
		log::out << log::RED<<"ObjectInfoRegistry - Serialization is not implemented\n";
	}
	void ObjectInfoRegistry::deserialize() {
		using namespace engone;
		log::out << log::RED << "ObjectInfoRegistry - Deserialization is not implemented\n";
	}

	uint32 ObjectInfoRegistry::registerItemInfo() {
		return m_itemInfos.add({})+1;
	}
	ObjectItemInfo& ObjectInfoRegistry::getItemInfo(uint32 dataIndex) {
		return *m_itemInfos.get(dataIndex-1);
	}
	void ObjectInfoRegistry::unregisterItemInfo(uint32 dataIndex) {
		// Todo: If ComplexData is handled with registries then it needs to be unregistered here.
		//		ComplexData is a member of Item so it is fine right now (2023-0-19).
		m_itemInfos.remove(dataIndex - 1);
	}

	uint32 ObjectInfoRegistry::registerWeaponInfo() {
		return m_weaponInfos.add({}) + 1;
	}
	ObjectWeaponInfo& ObjectInfoRegistry::getWeaponInfo(uint32 dataIndex) {
		return *m_weaponInfos.get(dataIndex - 1);
	}
	void ObjectInfoRegistry::unregisterWeaponInfo(uint32 dataIndex) {
		m_weaponInfos.remove(dataIndex-1);
	}

	uint32 ObjectInfoRegistry::registerCreatureInfo(const std::string& name) {
		return m_creatureInfos.add({name}) + 1;
	}
	ObjectCreatureInfo& ObjectInfoRegistry::getCreatureInfo(uint32 dataIndex) {
		return *m_creatureInfos.get(dataIndex - 1);
	}
	void ObjectInfoRegistry::unregisterCreatureInfo(uint32 dataIndex) {
		Session* session = ((GameApp*)engone::GetActiveWindow()->getParent())->getActiveSession();
		auto oinfo = m_creatureInfos.get(dataIndex-1);
		if (oinfo->inventoryDataIndex)
			session->inventoryRegistry.destroyInventory(oinfo->inventoryDataIndex);

		m_creatureInfos.remove(dataIndex-1);
	}
}