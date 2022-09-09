#include "Engone/GameObject.h"
#include "Engone/Engone.h"

namespace engone {
#ifdef ENGONE_PHYSICS
	void GameObject::loadColliders(GameGround* ground) {
		
		if (!modelAsset) {
			log::out << log::RED << "ModelAsset is null\n";
			return;
		}
		if (!rigidBody) {
			log::out << log::RED << "Rigidbody is null\n";
			return;
		}
		if (!modelAsset->valid()) {
			pendingColliders = true; // try again in update loop of engine.
			return;
		}
		pendingColliders = false;

		std::vector<glm::mat4> transforms = modelAsset->getParentTransforms(nullptr);

		bool noColliders = true;
		for (int i = 0; i < modelAsset->instances.size(); i++) {
			AssetInstance& inst = modelAsset->instances[i];
			//log::out << inst.name << "\n";
			//log::out <<loc << "\n";
			if (inst.asset->type == ColliderAsset::TYPE) {
				noColliders = false;
				ColliderAsset* asset = (ColliderAsset*)inst.asset;
				glm::mat4 loc = transforms[i]*inst.localMat;
				glm::vec3 matScale;
				rp3d::Transform tr = ToTransform(loc,&matScale);
				rp3d::CollisionShape* shape = nullptr;

				if (asset->colliderType == ColliderAsset::MapType) {
					glm::vec3 scale = asset->map.scale;
					scale.x*=matScale.x;
					scale.y*=matScale.y;
					scale.z*=matScale.z;
					shape = ground->m_pCommon->createHeightFieldShape(asset->map.gridColumns,
						asset->map.gridRows, asset->map.minHeight, asset->map.maxHeight,
						asset->map.heights.data(), rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE,
						1, 1, *(rp3d::Vector3*)&scale);

				} else if (asset->colliderType == ColliderAsset::CubeType) {
					glm::mat4 loc = transforms[i] * inst.localMat;
					rp3d::Vector3 scale = *(rp3d::Vector3*)&asset->cube.size;
					scale.x*=matScale.x;
					scale.y*=matScale.y;
					scale.z*=matScale.z;
					shape = ground->m_pCommon->createBoxShape(scale);
				}
				else if (asset->colliderType == ColliderAsset::CapsuleType) {
					float realHeight = asset->capsule.height - asset->capsule.radius * 2;
					realHeight *= matScale.y;
					float radius = asset->capsule.radius;
					radius *= (matScale.x + matScale.z) / 2;
					shape = ground->m_pCommon->createCapsuleShape(radius, realHeight);
				}
				else if (asset->colliderType == ColliderAsset::SphereType) {
					float radius = asset->sphere.radius;
					radius *= (matScale.x + matScale.y + matScale.z) / 3.f;
					shape = ground->m_pCommon->createSphereShape(radius);
				}

				rigidBody->addCollider(shape, tr);
				auto col = rigidBody->getCollider(rigidBody->getNbColliders() - 1);
				col->getMaterial().setFrictionCoefficient(0.5f);
				col->getMaterial().setBounciness(0.0f);
			}
		}
		if (noColliders) {
			// updating center of mass would cause wierd stuff
			log::out << "Loaded no colliders\n";
		} else {
			rigidBody->updateLocalInertiaTensorFromColliders();
			rigidBody->updateLocalCenterOfMassFromColliders();
		}
	}
	void GameObject::setOnlyTrigger(bool yes) {
		if (rigidBody) {
			for (int i = 0; i < rigidBody->getNbColliders(); i++) {
				auto col = rigidBody->getCollider(i);
				col->setIsTrigger(yes);
			}
		}
	}
#endif
}