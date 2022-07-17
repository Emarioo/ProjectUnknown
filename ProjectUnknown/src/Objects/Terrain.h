#pragma once

#include "Engone/Engone.h"

class Terrain : public engone::GameObject {
public:
	Terrain(engone::Engone* engone) : GameObject(engone) {
		 
		engone::Assets* assets = engone::GetActiveWindow()->getAssets();
		modelAsset = assets->set<engone::ModelAsset>("Terrain/Terrain");

		rp3d::Transform t;
		rigidBody = engone->m_pWorld->createRigidBody(t);
		rigidBody->setType(rp3d::BodyType::STATIC);

		for (auto& inst : modelAsset->instances) {
			if (inst.asset->type == engone::ColliderAsset::TYPE) {
				engone::ColliderAsset* asset = inst.asset->cast<engone::ColliderAsset>();
				if (asset->colliderType == engone::ColliderAsset::Type::HeightMap) {

					rp3d::HeightFieldShape* map = engone->m_pCommon->createHeightFieldShape(asset->heightMap.gridWidth,
						asset->heightMap.gridHeight,asset->heightMap.minHeight,asset->heightMap.maxHeight,
						asset->heightMap.heights.data(), rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE,
						1,1,*(rp3d::Vector3*)&asset->heightMap.scale);
				
					rp3d::Transform tr;
					tr.setFromOpenGL((float*)&inst.localMat);
					rigidBody->addCollider(map, tr);
					auto col = rigidBody->getCollider(rigidBody->getNbColliders() - 1);
					col->getMaterial().setFrictionCoefficient(3.f);
					col->getMaterial().setBounciness(0.1f);
				}
			}
		}
	}

private:
};