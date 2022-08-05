#pragma once

#include "Engone/Engone.h"

class Terrain : public engone::GameObject {
public:
	Terrain(engone::Engone* engone) : GameObject(engone) {
		using namespace engone;
		engone::Assets* assets = engone::GetActiveWindow()->getAssets();
		//modelAsset = assets->set<engone::ModelAsset>("Terrain/Terrain");
		modelAsset = assets->set<engone::ModelAsset>("C_floor/C_floor");
		//modelAsset = assets->set<engone::ModelAsset>("C_Test/C_Test");
		//modelAsset = assets->set<engone::ModelAsset>("C_other/C_other");
		//modelAsset = assets->set<engone::ModelAsset>("Other/Other");

		rp3d::Transform t;
		rigidBody = engone->m_pWorld->createRigidBody(t);
		rigidBody->setType(rp3d::BodyType::STATIC);
		//rigidBody->setLinearVelocity({0,1,1});

		loadColliders(engone);
	}

private:
};