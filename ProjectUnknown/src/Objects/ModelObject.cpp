#include "gonpch.h"

#include "ModelObject.h"

ModelObject::ModelObject(float x, float y, float z, engone::ModelAsset* model) 
	: GameObject("ModelObject",x, y, z) {
	
	renderComponent.SetModel(model);

	//renderComponent.animator.Enable("Leaf.003","RISE", { true, 1, 1 });
	//renderComponent.animator.Enable("Leaf2.001","SidePlantWind.004", { true, 1, 1 });
	//renderComponent.animator.Enable("PlantArmature.001","PlantWind.002", { true, 1, 1 });
	//renderComponent.animator.Enable("Leaf.003","RISE", { true, 1, 1 });
	//renderComponent.animator.Enable("Leaf2.001","SidePlantWind.003", { true, 1, 1 });
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