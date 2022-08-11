#pragma once

#include "Engone/Engone.h"

class Sword : public engone::GameObject {
public:
	Sword(engone::Engone* engone) : GameObject(engone) {

		engone::Assets* assets = engone::GetActiveWindow()->getAssets();
		modelAsset = assets->set<engone::ModelAsset>("SwordBase/SwordBase");
		
		rp3d::Transform t;
		collisionBody = engone->m_pWorld->createCollisionBody(t);

		rigidBody = engone->m_pWorld->createRigidBody(t);
		rigidBody->setType(rp3d::BodyType::DYNAMIC);
		rigidBody->setIsAllowedToSleep(false);

		loadColliders(engone);
	}

private:
};