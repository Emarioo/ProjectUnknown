#pragma once

#include "Engone/Engone.h"

class Sword : public engone::GameObject {
public:
	Sword(engone::Engone* engone) : GameObject(engone) {

		engone::Assets* assets = engone::GetActiveWindow()->getAssets();
		modelAsset = assets->set<engone::ModelAsset>("SwordBase/SwordBase");
		
		rp3d::Transform t;
		collisionBody = engone->m_pWorld->createCollisionBody(t);

		//rigidBody = engone->m_pWorld->createRigidBody(t);
		//rigidBody->setType(rp3d::BodyType::STATIC);

		for (auto& inst : modelAsset->instances) {
			if (inst.asset->type == engone::ColliderAsset::TYPE) {
				engone::ColliderAsset* asset = inst.asset->cast<engone::ColliderAsset>();
				if (asset->colliderType == engone::ColliderAsset::Type::Cube) {
					rp3d::Vector3 scale = *(rp3d::Vector3*)&asset->cube.size;
					rp3d::BoxShape* box = engone->m_pCommon->createBoxShape(scale);

					rp3d::Transform tr;
					tr.setFromOpenGL((float*)&inst.localMat);
					collisionBody->addCollider(box, tr);
					//rigidBody->addCollider(box, tr);
				}
			}
		}
	}

private:
};