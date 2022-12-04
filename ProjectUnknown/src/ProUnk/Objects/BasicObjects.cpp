#include "ProUnk/Objects/BasicObjects.h"

//#include "ProUnk/G"
#include "Engone/Engone.h"
#include "ProUnk/GameApp.h"

namespace prounk {

	engone::EngineObject* CreateObject(int type, World* world, engone::UUID uuid) {
		using namespace engone;
		if (type == OBJECT_DUMMY) return CreateDummy(world,uuid);
		if (type == OBJECT_SWORD) return CreateSword(world, uuid);
		if (type == OBJECT_TERRAIN) return CreateTerrain(world, uuid);
		if (type == OBJECT_PLAYER) return CreatePlayer(world, uuid);
		log::out << log::RED << "CreateObject - type '"<<type<<"' doesn't exist\n";
		return nullptr;
	}
	void DeleteObject(World* world, engone::EngineObject* object) {
		if (object->rigidBody) {
			world->m_pWorld->destroyRigidBody(object->rigidBody);
		}
		delete object;
	}
	engone::EngineObject* CreateDummy(World* world, engone::UUID uuid) {
		using namespace engone;

		EngineObject* out = new EngineObject(uuid);
		out->objectType = OBJECT_DUMMY;
		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();

		out->createRigidBody(world);
		out->setModel(assets->load<engone::ModelAsset>("Dummy/Dummy"));

		out->rigidBody->setType(rp3d::BodyType::DYNAMIC);
		out->rigidBody->setIsAllowedToSleep(false);
		out->rigidBody->enableGravity(false);

		CombatData* data = new CombatData();
		data->owner = out;
		data->totalFlatAtk = 1;

		int id = world->entityHandler.addEntry();
		world->entityHandler.getEntry(id).combatData = data;
		out->userData = id;

		out->flags |= OBJECT_HAS_COMBATDATA;

		out->rigidBody->setUserData(out);

		//out->setOnlyTrigger(true);
		out->setColliderUserData((void*)(COLLIDER_IS_HEALTH|COLLIDER_IS_DAMAGE));
		out->loadColliders();
		return out;
	}
	engone::EngineObject* CreateSword(World* world, engone::UUID uuid) {
		using namespace engone;

		EngineObject* out = new EngineObject(uuid);
		out->objectType = OBJECT_SWORD;

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->createRigidBody(world);
		out->setModel(assets->load<engone::ModelAsset>("SwordBase/SwordBase"));
		//out->setModel(assets->load<engone::ModelAsset>("Pickaxe/Pickaxe"));

		out->rigidBody->setType(rp3d::BodyType::DYNAMIC);
		out->rigidBody->setIsAllowedToSleep(false);

		out->rigidBody->setUserData(out);

		out->loadColliders();

		return out;
	}
	engone::EngineObject* CreateTerrain(World* world, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = new EngineObject(uuid);
		out->objectType = OBJECT_TERRAIN;

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->createRigidBody(world);
		out->setModel(assets->load<engone::ModelAsset>("Platform/Platform"));

		out->rigidBody->setType(rp3d::BodyType::STATIC);

		out->rigidBody->setUserData(out);

		out->loadColliders();
		return out;
	}
	engone::EngineObject* CreatePlayer(World* world, engone::UUID uuid) {
		using namespace engone;
		EngineObject* out = new EngineObject(uuid);
		out->objectType = OBJECT_PLAYER;

		CombatData* data = new CombatData();
		data->owner = out;

		int id = world->entityHandler.addEntry();
		world->entityHandler.getEntry(id).combatData = data;
		out->userData = id;

		out->flags |= OBJECT_HAS_COMBATDATA;

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->createRigidBody(world);
		out->setModel(assets->load<engone::ModelAsset>("Player/Player"));

		out->rigidBody->setType(rp3d::BodyType::DYNAMIC);
		out->rigidBody->setAngularLockAxisFactor({ 0,1,0 }); // only allow spin (y rotation)
		out->rigidBody->setIsAllowedToSleep(false);

		out->rigidBody->setUserData(out);

		out->setColliderUserData((void*)COLLIDER_IS_HEALTH);
		out->loadColliders();
		return out;
	}
}