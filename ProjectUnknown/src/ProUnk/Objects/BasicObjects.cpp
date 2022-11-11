#include "ProUnk/Objects/BasicObjects.h"

//#include "ProUnk/G"
#include "Engone/Engone.h"
#include "ProUnk/GameApp.h"

namespace prounk {

	engone::GameObject* CreateObject(int type, engone::GameGround* ground, engone::UUID uuid) {
		using namespace engone;
		if (type == OBJECT_DUMMY) return CreateDummy(ground,uuid);
		if (type == OBJECT_SWORD) return CreateSword(ground, uuid);
		if (type == OBJECT_TERRAIN) return CreateTerrain(ground, uuid);
		if (type == OBJECT_PLAYER) return CreatePlayer(ground, uuid);
		log::out << log::RED << "CreateObject - type '"<<type<<"' doesn't exist\n";
		return nullptr;
	}
	void DeleteObject(engone::GameGround* ground, engone::GameObject* object) {
		if (object->rigidBody) {
			ground->m_pWorld->destroyRigidBody(object->rigidBody);
		}
		delete object;
	}
	engone::GameObject* CreateDummy(engone::GameGround* ground, engone::UUID uuid) {
		using namespace engone;

		GameObject* out = new GameObject(uuid);
		out->objectType = OBJECT_DUMMY;
		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->modelAsset = assets->load<engone::ModelAsset>("Dummy/Dummy");
		out->animator.asset = out->modelAsset;

		rp3d::Transform t;
		out->rigidBody = ground->m_pWorld->createRigidBody(t);
		out->rigidBody->setType(rp3d::BodyType::DYNAMIC);
		out->rigidBody->setIsAllowedToSleep(false);
		out->rigidBody->enableGravity(false);

		CombatData* data = new CombatData();
		data->owner = out;
		out->userData = data;
		out->flags |= OBJECT_HAS_COMBATDATA;
		data->totalFlatAtk = 1;

		out->rigidBody->setUserData(out);

		//out->setOnlyTrigger(true);
		out->setColliderUserData((void*)(COLLIDER_IS_HEALTH|COLLIDER_IS_DAMAGE));
		out->loadColliders();
		return out;
	}
	//engone::GameObject* CreateSlime(engone::GameGround* ground, engone::UUID uuid = 0) {
	//	using namespace engone;

	//	GameObject* out = new GameObject(uuid);
	//	out->objectType = OBJECT_SLIME;
	//	engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
	//	out->modelAsset = assets->load<engone::ModelAsset>("Dummy/Dummy");
	//	out->animator.asset = out->modelAsset;

	//	rp3d::Transform t;
	//	out->rigidBody = ground->m_pWorld->createRigidBody(t);
	//	out->rigidBody->setType(rp3d::BodyType::DYNAMIC);
	//	out->rigidBody->setIsAllowedToSleep(false);
	//	//rigidBody->enableGravity(false);


	//	CombatData* data = new CombatData();
	//	//data->owner = this;
	//	out->userData = data;
	//	out->flags |= OBJECT_HAS_COMBATDATA;

	//	out->rigidBody->setUserData(out);

	//	out->setColliderUserData((void*)COLLIDER_IS_HEALTH);
	//	out->loadColliders();
	//	return out;
	//}
	engone::GameObject* CreateSword(engone::GameGround* ground, engone::UUID uuid) {
		using namespace engone;

		GameObject* out = new GameObject(uuid);
		out->objectType = OBJECT_SWORD;

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->modelAsset = assets->load<engone::ModelAsset>("SwordBase/SwordBase");
		out->animator.asset = out->modelAsset;

		rp3d::Transform t;
		out->rigidBody = ground->m_pWorld->createRigidBody(t);
		out->rigidBody->setType(rp3d::BodyType::DYNAMIC);
		out->rigidBody->setIsAllowedToSleep(false);

		out->rigidBody->setUserData(out);

		out->loadColliders();

		return out;
	}
	engone::GameObject* CreateTerrain(engone::GameGround* ground, engone::UUID uuid) {
		using namespace engone;
		GameObject* out = new GameObject(uuid);
		out->objectType = OBJECT_TERRAIN;

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->modelAsset = assets->load<engone::ModelAsset>("Platform/Platform");
		out->animator.asset = out->modelAsset;

		rp3d::Transform t;
		out->rigidBody = ground->m_pWorld->createRigidBody(t);
		out->rigidBody->setType(rp3d::BodyType::STATIC);

		out->rigidBody->setUserData(out);

		out->loadColliders();
		return out;
	}
	engone::GameObject* CreatePlayer(engone::GameGround* ground, engone::UUID uuid) {
		using namespace engone;
		GameObject* out = new GameObject(uuid);
		out->objectType = OBJECT_PLAYER;

		CombatData* data = new CombatData();
		data->owner = out;
		out->userData = data;
		out->flags |= OBJECT_HAS_COMBATDATA;

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		out->modelAsset = assets->load<engone::ModelAsset>("Player/Player");
		out->animator.asset = out->modelAsset;

		rp3d::Transform t;
		out->rigidBody = ground->m_pWorld->createRigidBody(t);
		out->rigidBody->setType(rp3d::BodyType::DYNAMIC);
		out->rigidBody->setAngularLockAxisFactor({ 0,1,0 }); // only allow spin (y rotation)
		out->rigidBody->setIsAllowedToSleep(false);

		out->rigidBody->setUserData(out);

		out->setColliderUserData((void*)COLLIDER_IS_HEALTH);
		out->loadColliders();
		return out;
	}
}