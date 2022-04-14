
#include "ModelObject.h"

ModelObject::ModelObject(float x, float y, float z, engone::ModelAsset* model) 
	: GameObject("ModelObject",x, y, z) {
	
	//SetModel(model);

	//animator.Enable(model, "Leaf.003","RISE", { true, 1, 1 });
	//animator.Enable(model, "Leaf2.001","SidePlantWind.004", { true, 1, 1 });
	//animator.Enable(model, "PlantArmature.001","PlantWind.002", { true, 1, 1 });
	//animator.Enable(model, "Leaf.003","RISE", { true, 1, 1 });
	//animator.Enable(model, "Leaf2.001","SidePlantWind.003", { true, 1, 1 });
}
//void ModelObject::Update(float delta) {
//	
//	/*if (engine::IsKey(GLFW_KEY_W)) {
//		if (blending < 1 && speed>0)
//			blending += speed * delta;
//	} else {
//		if (blending > 0)
//			blending -= speed * delta;
//	}
//
//	animator.Blend("goblin_idle",blending);
//	animator.Blend("goblin_run",1-blending);*/
//	
//	//animator.Blend("charlesBone", 1);
//	//animator.Speed("charlesBone", 1);
//
//	// animator.Update(delta, GetModel());
//}