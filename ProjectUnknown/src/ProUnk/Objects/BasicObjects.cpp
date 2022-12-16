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
		using namespace engone;
		if (object->getRigidBody()) {
			world->getPhysicsWorld()->destroyRigidBody(object->getRigidBody());
		}
		log::out <<log::RED<< __FILE__ << " Memory leak!\n";
		delete object;
	}
	engone::EngineObject* CreateDummy(World* world, engone::UUID uuid) {
		using namespace engone;

		EngineObject* obj = world->createObject(uuid);

		obj->setObjectType(OBJECT_DUMMY);

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();

		obj->setModel(assets->load<engone::ModelAsset>("Dummy/Dummy"));

		obj->getRigidBody()->setType(rp3d::BodyType::DYNAMIC);
		obj->getRigidBody()->setIsAllowedToSleep(false);
		obj->getRigidBody()->enableGravity(false);

		CombatData* data = new CombatData();
		data->owner = obj;
		data->totalFlatAtk = 1;

		

		int id = world->entityRegistry.addEntry();
		world->entityRegistry.getEntry(id).combatData = data;
		obj->userData = id;

		obj->flags |= OBJECT_HAS_COMBATDATA;

		obj->rigidBody->setUserData(out);

		//out->setOnlyTrigger(true);
		obj->setColliderUserData((void*)(COLLIDER_IS_HEALTH|COLLIDER_IS_DAMAGE));
		//obj->loadColliders();
		return obj;
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

		int id = world->entityRegistry.addEntry();
		world->entityRegistry.getEntry(id).combatData = data;
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