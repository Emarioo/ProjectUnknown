#pragma once

#include "Engone/Components/Entity.h"

class Tree : public engone::Entity {
public:
	Tree() : Entity(engone::ComponentEnum::Transform | engone::ComponentEnum::ModelRenderer) {}
	void Init() override {
		engone::ModelRenderer* m = getComponent<engone::ModelRenderer>();
		//engone::Physics* p = getComponent<engone::Physics>();
		m->asset = engone::GetAsset<engone::ModelAsset>("Oak/Oak");
		//p->renderCollision = true;
	}

	/*void Update(float delta) override {
		engone::log::out << "tree!\n";
	}*/
};