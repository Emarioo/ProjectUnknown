#pragma once

#include "Engone/Components/Entity.h"
#include "Engone/Components/System.h"

class Goblin : public engone::Entity, public engone::System {
private:
	float blending = 0;
	float speed = 1.7f;

public:
	Goblin() : engone::Entity(engone::Transform::ID | engone::Physics::ID |
		engone::ModelRenderer::ID) { }
	void Init() override;
	void OnUpdate(float delta) override;
	void OnCollision(engone::Collider& my, engone::Collider& their) override;

};