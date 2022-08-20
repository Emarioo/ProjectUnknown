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
		//rigidBody->enableGravity(false);

		loadColliders(engone);

		for (int i = 0; i < rigidBody->getNbColliders(); i++) {
			rigidBody->getCollider(i)->setUserData(&defenseData);
		}
	}
	void update(engone::UpdateInfo& info) override {
		using namespace engone;
		defenseData.update(info);
		//
		//glm::vec3 acc = glm::vec3(0, 0, 0);

		//glm::vec3 vel = ToGlmVec3(rigidBody->getLinearVelocity());
		//glm::vec3 pos = ToGlmVec3(rigidBody->getTransform().getPosition());

		////double x = sqrt(double(vel.x)*double(vel.x)+ double(vel.y) * double(vel.y)+ double(vel.z) * double(vel.z));

		//float velDist = glm::length(vel);

		//glm::vec3 focus = glm::vec3(0, 0, 0) - pos;

		//float radius = glm::length(focus);
		////log::out << focus << vel << "\n";
		////log::out << radius << " " << velDist << "\n";
		////acc += focus/radius*velDist*velDist/radius;
		//acc = focus/radius * velDist * velDist / radius;
		//
		//rp3d::Vector3 temp = ToRp3dVec3(acc);

		//rigidBody->applyLocalForceAtCenterOfMass(temp);
	}
	DefenseData defenseData;

private:
};