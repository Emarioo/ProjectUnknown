#include "ProUnk/Objects/BasicObjects.h"

#include "Engone/Engone.h"
#include "ProUnk/GameApp.h"

namespace prounk {

	bool HasCombatData(int type) {
		int types[]{ OBJECT_DUMMY,OBJECT_PLAYER,OBJECT_WEAPON };
		for (int i = 0; i < sizeof(types)/sizeof(int); i++) {
			if (type == types[i])
				return true;
		}
		return false;
	}
	//CombatData* GetCombatData(Dimension* dim, engone::EngineObject* object) {
	//	int type = object->getObjectType();
	//	if (type == OBJECT_DUMMY || type == OBJECT_PLAYER) {
	//		return &dim->getParent()->objectInfoRegistry.getCreatureInfo(object->getObjectInfo()).combatData;
	//	}else if (type == OBJECT_WEAPON) {
	//		return &dim->getParent()->objectInfoRegistry.getWeaponInfo(object->getObjectInfo()).combatData;
	//	}
	//	return nullptr;
	//}
	CombatData* GetCombatData(Session* session, engone::EngineObject* object) {
		//if (!object||!session)
		//	return nullptr;
		int type = object->getObjectType();
		if (type == OBJECT_DUMMY || type == OBJECT_PLAYER) {
			return &session->objectInfoRegistry.getCreatureInfo(object->getObjectInfo()).combatData;
		} else if (type == OBJECT_WEAPON) {
			return &session->objectInfoRegistry.getWeaponInfo(object->getObjectInfo()).combatData;
		}
		return nullptr;
	}
	engone::EngineObject* CreateObject(int type, Dimension* dimension, engone::UUID uuid) {
		using namespace engone;
		if (type == OBJECT_DUMMY) return CreateDummy(dimension,uuid);
		if (type == OBJECT_SWORD) return CreateSword(dimension, uuid);
		if (type == OBJECT_TERRAIN) return CreateTerrain(dimension, uuid);
		if (type == OBJECT_PLAYER) return CreatePlayer(dimension, uuid);
		log::out << log::RED << "CreateObject - type '"<<type<<"' doesn't exist\n";
		return nullptr;
	}
	void DeleteObject(Dimension* dimension, engone::EngineObject* object) {
		using namespace engone;
		//if (object->getRigidBody()) {
		//	dimension->getWorld()->getPhysicsWorld()->destroyRigidBody(object->getRigidBody());
		//}
		dimension->getWorld()->deleteObject(object);
		log::out <<log::RED<< __FILE__ << ":"<<(int)__LINE__ << " Memory leak!\n";
		// object info is not cleared. Inventory, combat data for example.
	}
	engone::EngineObject* CreateDummy(Dimension* dimension, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = dimension->getWorld()->createObject(uuid);
		out->setObjectType(OBJECT_DUMMY);

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->setModel(assets->load<engone::ModelAsset>("Dummy/Dummy"));

		out->createAnimator();
			
		int id = dimension->getParent()->objectInfoRegistry.registerCreatureInfo();
		out->setObjectInfo(id);

		CombatData& combatData = dimension->getParent()->objectInfoRegistry.getCreatureInfo(id).combatData;
		combatData.owner = out;
		combatData.damageType = CombatData::CONTINOUS_DAMAGE;
		combatData.damagePerSecond = 20;
		combatData.knockStrength = 0.1f;

		out->getRigidBody()->setType(rp3d::BodyType::DYNAMIC);
		out->getRigidBody()->enableGravity(false);
		//out->setOnlyTrigger(true);
		out->setColliderUserData((void*)(COLLIDER_IS_HEALTH|COLLIDER_IS_DAMAGE));
		out->loadColliders();
		return out;
	}
	engone::EngineObject* CreateSword(Dimension* dimension, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = dimension->getWorld()->createObject(uuid);
		out->setObjectType(OBJECT_SWORD);

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->setModel(assets->load<engone::ModelAsset>("SwordBase/SwordBase"));
		//out->setModel(assets->load<engone::ModelAsset>("Pickaxe/Pickaxe"));

		out->getRigidBody()->setType(rp3d::BodyType::DYNAMIC);
		out->loadColliders();
		return out;
	}
	engone::EngineObject* CreateTerrain(Dimension* dimension, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = dimension->getWorld()->createObject(uuid);
		out->setObjectType(OBJECT_TERRAIN);

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->setModel(assets->load<engone::ModelAsset>("Platform/Platform"));

		out->getRigidBody()->setType(rp3d::BodyType::STATIC);
		out->loadColliders();
		return out;
	}
	engone::EngineObject* CreatePlayer(Dimension* dimension, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = dimension->getWorld()->createObject(uuid);
		out->setObjectType(OBJECT_PLAYER);

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->setModel(assets->load<engone::ModelAsset>("Player/Player"));
		
		out->createAnimator();

		int id = dimension->getParent()->objectInfoRegistry.registerCreatureInfo();
		out->setObjectInfo(id);
		
		CombatData& combatData = dimension->getParent()->objectInfoRegistry.getCreatureInfo(id).combatData;
		combatData.owner = out;

		out->getRigidBody()->setType(rp3d::BodyType::DYNAMIC);
		out->getRigidBody()->setAngularLockAxisFactor({0,1,0}); // only allow spin (y rotation)
		out->setColliderUserData((void*)COLLIDER_IS_HEALTH);
		out->loadColliders();
		return out;
	}
	engone::EngineObject* CreateWeapon(Dimension* dimension, Item& item, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = dimension->getWorld()->createObject(uuid);
		out->setObjectType(OBJECT_WEAPON);
		out->setModel(dimension->getParent()->modelRegistry.getModel(item.getModelId()));

		Session* session = dimension->getParent();

		int id = session->objectInfoRegistry.registerWeaponInfo();
		out->setObjectInfo(id);

		CombatData& combatData = session->objectInfoRegistry.getWeaponInfo(id).combatData;
		combatData.owner = out;

		// damage of combatData is customized by the coder
		
		out->getRigidBody()->setType(rp3d::BodyType::DYNAMIC);
		out->setColliderUserData((void*)COLLIDER_IS_DAMAGE);
		out->loadColliders(dimension->getWorld());
		return out;
	}
	engone::EngineObject* CreateItem(Dimension* dimension, Item& item, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = dimension->getWorld()->createObject(uuid);
		out->setObjectType(OBJECT_ITEM);

		//engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		//out->setModel(assets->load<engone::ModelAsset>("SwordBase/SwordBase"));

		ModelAsset* model = dimension->getParent()->modelRegistry.getModel(item.getModelId());
		out->setModel(model);

		Session* session = dimension->getParent();

		int id = session->objectInfoRegistry.registerItemInfo();
		out->setObjectInfo(id);

		session->objectInfoRegistry.getItemInfo(id).item = item;

		out->getRigidBody()->setType(rp3d::BodyType::DYNAMIC);
		//out->setColliderUserData((void*)COLLIDER_IS_DAMAGE);
		out->loadColliders(dimension->getWorld());
		return out;
	}
}