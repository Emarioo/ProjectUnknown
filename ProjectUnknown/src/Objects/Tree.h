#pragma once

#include "Engone/Components/Entity.h"

class Tree : public engone::Entity {
public:
	Tree() : Entity(engone::ComponentEnum::Transform | engone::ComponentEnum::Model) {}
	void Init() override {
		engone::Model* m = getComponent<engone::Model>();
		//engone::Physics* p = getComponent<engone::Physics>();
		m->modelAsset = engone::GetAsset<engone::ModelAsset>("Oak/Oak");
		//p->renderCollision = true;
	}

	/*void Update(float delta) override {
		engone::log::out << "tree!\n";
	}*/
};