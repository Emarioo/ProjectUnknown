#include "ProUnk/Registries/ObjectInfoRegistry.h"

namespace prounk {

	void ObjectInfoRegistry::serialize() {
		using namespace engone;
		log::out << log::RED<<"ObjectInfoRegistry - serialization not implemented\n";
	}
	void ObjectInfoRegistry::deserialize() {
		using namespace engone;
		log::out << log::RED << "ObjectInfoRegistry - serialization not implemented\n";
	}
	ObjectItemInfo& ObjectInfoRegistry::getItemInfo(int id) {
		return m_itemInfos[id - 1];
	}
	int ObjectInfoRegistry::registerItemInfo() {
		m_itemInfos.push_back({});
		return m_itemInfos.size();
	}
	ObjectCreatureInfo& ObjectInfoRegistry::getCreatureInfo(int id) {
		return m_creatureInfos[id - 1];
	}
	int ObjectInfoRegistry::registerCreatureInfo() {
		m_creatureInfos.push_back({});
		return m_creatureInfos.size();
	}
	ObjectWeaponInfo& ObjectInfoRegistry::getWeaponInfo(int id) {
		return m_weaponInfos[id - 1];
	}
	int ObjectInfoRegistry::registerWeaponInfo() {
		m_weaponInfos.push_back({});
		return m_weaponInfos.size();
	}
}