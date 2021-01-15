#include "Tutorial.h"

Tutorial::Tutorial(float x, float y, float z) {
	SetPosition(x, y, z);

	renderComponent.AddMesh("Floor");

}
void Tutorial::PreComponents() {
	engine::Location body;
	body.Translate(position);
	body.Rotate(rotation);

	renderComponent.SetMatrix(0, body.mat());
}
void Tutorial::Update(float delta) {
	
}