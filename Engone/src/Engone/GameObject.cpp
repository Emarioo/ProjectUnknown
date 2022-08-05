#include "Engone/GameObject.h"
#include "Engone/Engone.h"

namespace engone {

	void GameObject::loadColliders(Engone* engone) {
		if (!modelAsset) {
			log::out << log::RED << "ModelAsset is null\n";
			return;
		}
		if (!rigidBody) {
			log::out << log::RED << "Rigidbody is null\n";
			return;
		}

		std::vector<glm::mat4> transforms = modelAsset->getParentTransforms(nullptr);

		for (int i = 0; i < modelAsset->instances.size(); i++) {
			engone::AssetInstance& inst = modelAsset->instances[i];
			if (inst.asset->type == engone::ColliderAsset::TYPE) {
				engone::ColliderAsset* asset = inst.asset->cast<engone::ColliderAsset>();
				glm::mat4 loc = transforms[i] * inst.localMat;
				rp3d::Transform tr = ToTransform(loc);
				rp3d::CollisionShape* shape = nullptr;

				if (asset->colliderType == engone::ColliderAsset::Type::HeightMap) {

					shape = engone->m_pCommon->createHeightFieldShape(asset->heightMap.gridWidth,
						asset->heightMap.gridHeight, asset->heightMap.minHeight, asset->heightMap.maxHeight,
						asset->heightMap.heights.data(), rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE,
						1, 1, *(rp3d::Vector3*)&asset->heightMap.scale);

				} else if (asset->colliderType == engone::ColliderAsset::Type::Cube) {
					glm::mat4 loc = transforms[i] * inst.localMat;
					rp3d::Vector3 scale = *(rp3d::Vector3*)&asset->cube.size;
					shape = engone->m_pCommon->createBoxShape(scale);
				} else if (asset->colliderType == engone::ColliderAsset::Type::Capsule) {
					shape = engone->m_pCommon->createCapsuleShape(asset->capsule.radius, asset->capsule.height);
				}

				rigidBody->addCollider(shape, tr);
				auto col = rigidBody->getCollider(rigidBody->getNbColliders() - 1);
				col->getMaterial().setFrictionCoefficient(0.f);
				col->getMaterial().setBounciness(0.f);
			}
		}
		rigidBody->updateLocalCenterOfMassFromColliders();
	}
	void GameObject::setOnlyTrigger(bool yes) {
		if (rigidBody) {
			for (int i = 0; i < rigidBody->getNbColliders(); i++) {
				auto col = rigidBody->getCollider(i);
				col->setIsTrigger(yes);
			}
		}
	}
}