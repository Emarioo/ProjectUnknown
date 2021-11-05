#include "gonpch.h"

#include "ModelObject.h"

ModelObject::ModelObject(float x, float y, float z, engone::ModelAsset* model) 
	: GameObject("ModelObject",x, y, z) {
	
	renderComponent.model = model;

	//renderComponent.animator.Enable("Quater", { true, 1, 1 });
}
void ModelObject::Update(float delta) {
	/*
	if (engine::IsKey(GLFW_KEY_W)) {
		if (blending < 1 && speed>0)
			blending += speed * delta;
	} else {
		if (blending > 0)
			blending -= speed * delta;
	}

	renderComponent.animator.Blend("goblin_idle",blending);
	renderComponent.animator.Blend("goblin_run",1-blending);
	*/
	//renderComponent.animator.Blend("charlesBone", 1);
	//renderComponent.animator.Speed("charlesBone", 1);

	//renderComponent.animator.Update(delta);
}