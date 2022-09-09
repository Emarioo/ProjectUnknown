#pragma once

#include "Engone/Engone.h"

class Sword : public engone::GameObject {
public:
	Sword(engone::GameGround* ground) : GameObject(ground) {

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		//engone::Assets* assets = engone::GetActiveWindow()->getAssets();
		//modelAsset = assets->set<engone::ModelAsset>("SwordBase/SwordBase");
		modelAsset = assets->load<engone::ModelAsset>("SwordBase/SwordBase");
		
		rp3d::Transform t;
		collisionBody = ground->m_pWorld->createCollisionBody(t);

		rigidBody = ground->m_pWorld->createRigidBody(t);
		rigidBody->setType(rp3d::BodyType::DYNAMIC);
		rigidBody->setIsAllowedToSleep(false);

		loadColliders(ground);
	}

private:
};