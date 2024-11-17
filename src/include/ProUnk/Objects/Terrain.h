#pragma once

#include "Engone/Engone.h"

class Terrain : public engone::GameObject {
public:
	Terrain(engone::GameGround* ground) : GameObject() {
		using namespace engone;
		//engone::Assets* assets = engone::GetActiveWindow()->getAssets();
		engone::AssetStorage* assets = engone::GetActiveWindow()->getStorage();
		//modelAsset = assets->set<engone::ModelAsset>("Terrain/Terrain");
		//modelAsset = assets->set<engone::ModelAsset>("C_floor/C_floor");
		//modelAsset = assets->set<engone::ModelAsset>("C_Test/C_Test");
		//modelAsset = assets->set<engone::ModelAsset>("Platform/Platform");
		modelAsset = assets->load<engone::ModelAsset>("Platform/Platform");
		//modelAsset = assets->set<engone::ModelAsset>("Other/Other");

		rp3d::Transform t;
		rigidBody = ground->m_pWorld->createRigidBody(t);
		rigidBody->setType(rp3d::BodyType::STATIC);
		//rigidBody->setLinearVelocity({0,1,1});

		loadColliders();
	}

private:
};