#pragma once

#include "Engone/Engone.h"

class Dummy : public engone::GameObject {
public:
	Dummy(engone::Engone* engone) : GameObject(engone) {

		engone::Assets* assets = engone::GetActiveWindow()->getAssets();
		modelAsset = assets->set<engone::ModelAsset>("Dummy/Dummy");

		rp3d::Transform t;
		collisionBody = engone->m_pWorld->createCollisionBody(t);

		rigidBody = engone->m_pWorld->createRigidBody(t);
		rigidBody->setType(rp3d::BodyType::DYNAMIC);
		rigidBody->setIsAllowedToSleep(false);

		loadColliders(engone);

		for (int i = 0; i < rigidBody->getNbColliders(); i++) {
			rigidBody->getCollider(i)->setUserData(&defenseData);
		}
	}
	void update(engone::UpdateInfo& info) override {
		defenseData.update(info);
	}

	DefenseData defenseData;

private:
};