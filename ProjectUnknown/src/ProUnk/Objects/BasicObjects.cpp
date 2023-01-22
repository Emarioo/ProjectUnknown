#include "ProUnk/Objects/BasicObjects.h"

#include "Engone/Engone.h"
#include "ProUnk/GameApp.h"

namespace prounk {
	const char* to_string(BasicObjectType value) {
		switch (value) {
		case OBJECT_TERRAIN: return "OBJECT_TERRAIN";
		case OBJECT_ITEM: return "OBJECT_ITEM";
		case OBJECT_WEAPON: return "OBJECT_WEAPON";
		case OBJECT_CREATURE: return "OBJECT_CREATURE";
		case OBJECT_PLAYER: return "OBJECT_PLAYER";
		case OBJECT_DUMMY: return "OBJECT_DUMMY";
		}
		return "Unknown";
	}
	engone::Logger& operator<<(engone::Logger& log, BasicObjectType value) {
		return log << to_string(value);
	}
	bool HasCombatData(int type) {
		int types[]{ OBJECT_CREATURE,OBJECT_WEAPON };
		for (int i = 0; i < sizeof(types)/sizeof(int); i++) {
			if (type & types[i])
				return true;
		}
		return false;
	}
	void DropInventory(engone::EngineObject* object, float shock) {
		using namespace engone;
		if (!object) return;
		Inventory* inv = GetInventory(object);
		if (!inv) return;

		//auto& dims = app->getActiveSession()->getDimensions();
		//if (dims.size() == 0) {
		//	releasePlayer(plr);
		//	return;
		//}

		// Drops the held object at a specific position
		Dimension* dim = ((Dimension*)object->getWorld()->getUserData()); // Todo: the dimension should be the one the player is in.
		//auto heldObject = requestHeldObject();
		if (inv->getSlotSize() == 0) {
			log::out << "DropInventory("<<(BasicObjectType)object->getObjectType()<<") : No items to drop\n";
		}
		for (int i = 0; i < inv->getSlotSize(); i++) {
			Item& item = inv->getItem(i);

			if (item.getType() == 0)
				continue;

			auto itemObject = CreateItem(dim, item);
			item = Item(); // clear slot

			// use a random position for items not equipped.
			glm::vec3 offset = { GetRandom() - .5f, 1 + GetRandom() - .5f,GetRandom() - .5f };
			offset *= .5;
			itemObject->setPosition(object->getPosition() + offset);

			dim->getWorld()->lockPhysics();
			rp3d::Vector3 force = { (float)GetRandom() - .5f, (float)GetRandom(),(float)GetRandom() - .5f };
			force *= 200.f * shock;
			itemObject->getRigidBody()->applyWorldForceAtCenterOfMass(force);

			rp3d::Vector3 torque = { (float)GetRandom() - .5f, (float)GetRandom() - .5f,(float)GetRandom() - .5f };
			torque *= 600.f * shock;
			itemObject->getRigidBody()->applyWorldForceAtCenterOfMass(torque);
			dim->getWorld()->unlockPhysics();

			dim->getParent()->netAddGeneral(itemObject);

			dim->getWorld()->releaseAccess(itemObject->getUUID());
		}
	}
	bool IsDead(engone::EngineObject* object) {
		if (!object) return false;
		int type = object->getObjectType();
		Session* session = ((Dimension*)object->getWorld()->getUserData())->getParent();
		if (type & OBJECT_CREATURE) {
			CombatData& combatData = session->objectInfoRegistry.getCreatureInfo(object->getObjectInfo()).combatData;
			return combatData.health == 0;
		}
		return false;
	}
	CombatData* GetCombatData(engone::EngineObject* object) {
		if (!object) return nullptr;
		int type = object->getObjectType();
		Session* session = ((Dimension*)object->getWorld()->getUserData())->getParent();
		if (type & OBJECT_CREATURE) {
			return &session->objectInfoRegistry.getCreatureInfo(object->getObjectInfo()).combatData;
		} else if (type & OBJECT_WEAPON) {
			return &session->objectInfoRegistry.getWeaponInfo(object->getObjectInfo()).combatData;
		}
		return nullptr;
	}
	Inventory* GetInventory(engone::EngineObject* object) {
		if (!object) return nullptr;
		int type = object->getObjectType();
		Session* session = ((Dimension*)object->getWorld()->getUserData())->getParent();
		if (type & OBJECT_CREATURE) {
			return session->inventoryRegistry.getInventory(session->objectInfoRegistry.getCreatureInfo(object->getObjectInfo()).inventoryDataIndex);
		} 
		return nullptr;
	}
	void DeleteObject(Dimension* dimension, engone::EngineObject* object) {
		using namespace engone;
		
		if (object->getObjectInfo() != 0) {
			auto& reg = dimension->getParent()->objectInfoRegistry;
			if (object->getObjectType() & OBJECT_ITEM) {
				reg.unregisterItemInfo(object->getObjectInfo());
				// Todo: Unregister complex data?
				//		At the time of writing ComplexDataRegistry isn't used. Instead ComplexData is a member inside Item.
			} else if (object->getObjectType() & OBJECT_WEAPON) {
				reg.unregisterWeaponInfo(object->getObjectInfo());
			} else if (object->getObjectType() & OBJECT_CREATURE) {
				reg.unregisterCreatureInfo(object->getObjectInfo());
			} else {
				goto J123; // object types here will not have it's info reset
			}
			object->setObjectInfo(0); // reset info of the unregistered infos
		J123: { }
		}
		log::out << "Delete " << (BasicObjectType)object->getObjectType() << " " << object->getUUID() << "\n";
		dimension->getWorld()->deleteObject(object->getUUID());

		//log::out <<log::RED<< __FILE__ << ":"<<(int)__LINE__ << " Memory leak!\n";

		// ObjectInfo could have allocations or registry entries that need to be dealt with.
		// Not doing so will result in memory leaks. It should be fine now (2023-01-19) but if new info types are added
		// then function needs updating.
	}
//#define LOG_CREATE_OBJ()
#define LOG_CREATE_OBJ() log::out << "Create " << (BasicObjectType)out->getObjectType() << " " << out->getUUID()<<"\n";

	engone::EngineObject* CreateDummy(Dimension* dimension, engone::UUID uuid) {
		using namespace engone;

		EngineObject* out = dimension->getWorld()->createObject(uuid);
		out->setObjectType(OBJECT_DUMMY);
		if (uuid == 0)
			out->setFlags(out->getFlags() | Session::OBJECT_NETMOVE);

		LOG_CREATE_OBJ()

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->setModel(assets->load<engone::ModelAsset>("Dummy/Dummy"));

		out->createAnimator();
			
		Session* session = dimension->getParent();

		uint32 dataIndex = session->objectInfoRegistry.registerCreatureInfo("Dummy");
		out->setObjectInfo(dataIndex);

		auto& oinfo = session->objectInfoRegistry.getCreatureInfo(dataIndex);
		CombatData& combatData = oinfo.combatData;
		combatData.owner = out;
		combatData.damageType = CombatData::CONTINOUS_DAMAGE;
		combatData.damagePerSecond = 20;
		combatData.knockStrength = 0.1f;

		int goldCount = 1+GetRandom()*3;
		if (GetRandom() > 0.95) // Rare chance for extra much gold
			goldCount += 5;
		
		if (goldCount > 0) {
			oinfo.inventoryDataIndex = session->inventoryRegistry.createInventory();
			Inventory* inv = session->inventoryRegistry.getInventory(oinfo.inventoryDataIndex);

			auto* goldType = session->itemTypeRegistry.getType("gold_ingot");
			if (goldType) {
				inv->resizeSlots(goldCount);
				for (int i = 0; i < goldCount; i++) {
					inv->getItem(i) = Item(goldType->itemType, 1);
				}
			} else {
				log::out << "CreateDummy : gold_ingot is not registered\n";
			}
		}

		out->getRigidBody()->setType(rp3d::BodyType::DYNAMIC);
		out->getRigidBody()->enableGravity(false);
		
		out->setColliderUserData((void*)(COLLIDER_IS_HEALTH|COLLIDER_IS_DAMAGE));
		out->loadColliders();
		return out;
	}
	engone::EngineObject* CreateTerrain(Dimension* dimension, const std::string& modelName, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = dimension->getWorld()->createObject(uuid);
		out->setObjectType(OBJECT_TERRAIN);
		if (uuid == 0)
			out->setFlags(out->getFlags() | Session::OBJECT_NETMOVE);

		LOG_CREATE_OBJ()

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->setModel(assets->load<engone::ModelAsset>(modelName));

		out->getRigidBody()->setType(rp3d::BodyType::STATIC);
		out->loadColliders();
		return out;
	}
	engone::EngineObject* CreatePlayer(Dimension* dimension, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = dimension->getWorld()->createObject(uuid);
		out->setObjectType(OBJECT_PLAYER);
		if (uuid == 0)
			out->setFlags(out->getFlags() | Session::OBJECT_NETMOVE);

		LOG_CREATE_OBJ()

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->setModel(assets->load<engone::ModelAsset>("Player/Player"));
		
		out->createAnimator();

		uint32 id = dimension->getParent()->objectInfoRegistry.registerCreatureInfo("Player");
		out->setObjectInfo(id);
		
		CombatData& combatData = dimension->getParent()->objectInfoRegistry.getCreatureInfo(id).combatData;
		combatData.owner = out;

		out->getRigidBody()->setType(rp3d::BodyType::DYNAMIC);
		out->getRigidBody()->setAngularLockAxisFactor({0,1,0}); // only allow spin (y rotation)
		out->setColliderUserData((void*)COLLIDER_IS_HEALTH);
		out->loadColliders();
		return out;
	}
	engone::EngineObject* CreateWeapon(Dimension* dimension, const std::string& modelName, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = dimension->getWorld()->createObject(uuid);
		out->setObjectType(OBJECT_WEAPON);
		if (uuid == 0)
			out->setFlags(out->getFlags() | Session::OBJECT_NETMOVE);

		LOG_CREATE_OBJ()

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->setModel(assets->load<engone::ModelAsset>(modelName));
		//out->setModel(dimension->getParent()->modelRegistry.getModel(item.getModelId()));

		Session* session = dimension->getParent();

		int id = session->objectInfoRegistry.registerWeaponInfo();
		out->setObjectInfo(id);

		CombatData& combatData = session->objectInfoRegistry.getWeaponInfo(id).combatData;
		combatData.owner = out;

		// damage of combatData is customized by the coder
		
		out->getRigidBody()->setType(rp3d::BodyType::DYNAMIC);
		out->setColliderUserData((void*)COLLIDER_IS_DAMAGE);
		out->loadColliders();
		return out;
	}
	engone::EngineObject* CreateItem(Dimension* dimension, Item& item, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = dimension->getWorld()->createObject(uuid);
		out->setObjectType(OBJECT_ITEM);

		LOG_CREATE_OBJ()

		if (uuid == 0)
			out->setFlags(out->getFlags() | Session::OBJECT_NETMOVE);
		//engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		//out->setModel(assets->load<engone::ModelAsset>("SwordBase/SwordBase"));

		ModelAsset* model = dimension->getParent()->modelRegistry.getModel(item.getModelId());
		out->setModel(model);

		Session* session = dimension->getParent();

		int id = session->objectInfoRegistry.registerItemInfo();
		out->setObjectInfo(id);

		session->objectInfoRegistry.getItemInfo(id).item = item;

		out->getRigidBody()->setType(rp3d::BodyType::DYNAMIC);
		out->loadColliders();
		return out;
	}
}