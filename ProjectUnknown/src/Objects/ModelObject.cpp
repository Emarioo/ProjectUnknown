#include "ModelObject.h"


ModelObject::ModelObject(float x, float y, float z) 
	: GameObject(x,y,z) {
	
	renderComponent.SetModel("George");

	renderComponent.animator.Enable("georgeBoneAction", { true, 1, 1 });
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
	//renderComponent.animator.Blend("goblin_idle", 1);
	//renderComponent.animator.Blend("goblin_run", 0);

	renderComponent.animator.Update(delta);
}