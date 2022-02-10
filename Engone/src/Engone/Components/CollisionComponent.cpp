#include "gonpch.h"

#include "CollisionComponent.h"

namespace engone {

	bool CollisionComponent::IsTriggered() {
		return m_isTriggered;
	}
	bool CollisionComponent::TestCollision(Collider& c1, Collider& c2) {
		bool colliding = false;
		if (c1.asset->colliderType == ColliderAsset::Type::Sphere &&
			c2.asset->colliderType == ColliderAsset::Type::Sphere) {
			float dist = (c1.asset->sphere.position - c2.asset->sphere.position).length();
			if (dist < c1.asset->sphere.radius + c2.asset->sphere.radius) {
				colliding = true;
			}

		} else if (c1.asset->colliderType == ColliderAsset::Type::Cube &&
			c2.asset->colliderType == ColliderAsset::Type::Cube) {

		}
		if (colliding) {
			if (m_isTrigger) {
				m_isTriggered = true;
				return false;
			}
		}
		return true;
	}

#if gone
	void CollisionComponent::UpdatePoints() {
		points.clear();
		if (coll != nullptr) {
			for (glm::vec3 p : coll->points) {
				Location loc;
				loc.Matrix(matrix);
				loc.Translate(p);
				points.push_back(loc.vec());
			}
		}
	}

	bool CollisionComponent::IsClose(CollisionComponent* c2) {
		//if(coll!=nullptr&&c2->coll!=nullptr)
			//return glm::length(*oPos - *c2->oPos) < d->furthestPoint + d2->furthestPoint;
		//return false;

		return true;
	}
	void CollisionComponent::SetCollider(const std::string& name) {
	//	coll = GetColliderAsset(name);
	//	isActive = coll != nullptr;
		UpdatePoints();
	}
	void CollisionComponent::SetMatrix(glm::mat4 mat) {
		matrix = mat;
		UpdatePoints();
	}
#endif
	/*
	Run collider.SetColl("a_name_which_is_not_used")
	before calling MakeCube(...)
	
	void CollisionComponent::MakeCube(float x, float y, float z, float w, float h, float d) {
		glm::vec3 O = glm::vec3(x, y, z);

		if (coll == nullptr)
			return;

		coll->furthestPoint = sqrt(pow(x + w / 2, 2) + pow(y + h / 2, 2) + pow(z + d / 2, 2));

		coll->points.push_back(O + glm::vec3(-w, -h, -d) / 2.f);
		coll->points.push_back(O + glm::vec3(w, -h, -d) / 2.f);
		coll->points.push_back(O + glm::vec3(w, -h, d) / 2.f);
		coll->points.push_back(O + glm::vec3(-w, -h, d) / 2.f);

		coll->points.push_back(O + glm::vec3(-w, h, -d) / 2.f);
		coll->points.push_back(O + glm::vec3(w, h, -d) / 2.f);
		coll->points.push_back(O + glm::vec3(w, h, d) / 2.f);
		coll->points.push_back(O + glm::vec3(-w, h, d) / 2.f);

		coll->quad.push_back(0);
		coll->quad.push_back(1);
		coll->quad.push_back(2);
		coll->quad.push_back(3);

		coll->quad.push_back(0);
		coll->quad.push_back(1);
		coll->quad.push_back(5);
		coll->quad.push_back(4);

		coll->quad.push_back(1);
		coll->quad.push_back(2);
		coll->quad.push_back(6);
		coll->quad.push_back(5);

		coll->quad.push_back(2);
		coll->quad.push_back(3);
		coll->quad.push_back(7);
		coll->quad.push_back(6);

		coll->quad.push_back(3);
		coll->quad.push_back(0);
		coll->quad.push_back(4);
		coll->quad.push_back(7);

		coll->quad.push_back(4);
		coll->quad.push_back(5);
		coll->quad.push_back(6);
		coll->quad.push_back(7);

		UpdatePoints();
	}*/
}