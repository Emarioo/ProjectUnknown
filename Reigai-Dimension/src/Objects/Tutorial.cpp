#include "Tutorial.h"

Tutorial::Tutorial(float x, float y, float z) {
	name = "Tutorial";
	SetPosition(x, y, z);

	renderComponent.SetModel("Terrain");
	collisionComponent.SetCollider(renderComponent.model->colliderName);

}
void Tutorial::Update(float delta) {
	
}