
#include "Gnorg.h"

Gnorg::Gnorg(float x, float y, float z) : GameObject("Gnorg",x,y,z) {
	/*
	renderComponent.SetAnim("goblin_slash");
	renderComponent.anim.running = true;
	renderComponent.anim.loop = true;
	*/
	//renderComponent.SetBone("goblin_skeleton");

	//renderComponent.SetModel("Gnorg");
	//renderComponent.animator.Enable("goblin_idle", { 0,true,0.5,1 });
	//renderComponent.animator.Enable("goblin_run", { 0,true,0.5,1 });
	//bug::out < renderComponent.model->animations[0]->name < bug::end;
	//renderComponent.animator.Disable("ArmatureAction");
	//renderComponent.animator.running = true;
	//renderComponent.animator.loop = true;
}
//float blending = 0;
//float speed = 1.7;
//void Gnorg::Update(float delta) {
//	/*if (engine::IsKey(GLFW_KEY_W)) {
//		if (blending < 1 && speed>0)
//			blending += speed * delta;
//	} else {
//		if (blending > 0)
//			blending -= speed * delta;
//	}*/
//
//	//renderComponent.animator.Blend("goblin_idle", blending);
//	//renderComponent.animator.Blend("goblin_run", 1 - blending);
//
//	//renderComponent.animator.Update(delta);
//}