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
	ObjectPlayerInfo& ObjectInfoRegistry::getPlayerInfo(int id) {
		return m_playerInfos[id - 1];
	}
	int ObjectInfoRegistry::registerItemInfo() {
		m_itemInfos.push_back({});
		return m_itemInfos.size();
	}
	int ObjectInfoRegistry::registerPlayerInfo() {
		m_playerInfos.push_back({});
		return m_playerInfos.size();
	}
}