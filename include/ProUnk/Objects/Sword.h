#pragma once

#include "Engone/Engone.h"

class Sword : public engone::GameObject {
public:
	Sword(engone::GameGround* ground) : GameObject() {

		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		modelAsset = assets->load<engone::ModelAsset>("SwordBase/SwordBase");
		
		rp3d::Transform t;
		collisionBody = ground->m_pWorld->createCollisionBody(t);

		rigidBody = ground->m_pWorld->createRigidBody(t);
		rigidBody->setType(rp3d::BodyType::DYNAMIC);
		rigidBody->setIsAllowedToSleep(false);

		loadColliders();
	}

private:
};