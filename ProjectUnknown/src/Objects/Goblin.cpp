#include "gonpch.h"

#include "Goblin.h"

void Goblin::Init() {
	engone::ModelRenderer* r = getComponent<engone::ModelRenderer>();
	r->asset = engone::GetAsset<engone::ModelAsset>("Hitbox/Hitbox");

	engone::Physics* p = getComponent<engone::Physics>();
	//p->movable = true;
	p->renderCollision = true;
}
void Goblin::OnUpdate(float delta) {

}
void Goblin::OnCollision(engone::Collider& my, engone::Collider& their) {

}