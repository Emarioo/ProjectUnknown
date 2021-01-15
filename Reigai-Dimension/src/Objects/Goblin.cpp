#include "Goblin.h"

Goblin::Goblin(float x, float y, float z) {
	SetPosition(x, y, z);
	/*
	renderComponent.SetAnim("goblin_slash");
	renderComponent.anim.running = true;
	renderComponent.anim.loop = true;
	*/
	//renderComponent.SetBone("goblin_skeleton");
	
	renderComponent.AddMesh("TestMesh");
}
void Goblin::PreComponents() {
	engine::Location body;
	body.Translate(position);
	body.Rotate(rotation);

	renderComponent.SetMatrix(0, body.mat());
}
void Goblin::Update(float delta) {
	renderComponent.anim.Update(delta);
	
}