#include "MagicStaff.h"

MagicStaff::MagicStaff(float x,float y,float z) {
	SetPosition(x, y, z);
	SetName("MagicStaff");

	renderComponent.AddMesh("staff");
	renderComponent.AddMesh("fireball");

}

void MagicStaff::PreComponents() {
	engine::Location body;
	body.Translate(GetPos());
	body.Rotate(GetRot());

	renderComponent.SetMatrix(0,body.mat());
	body.Translate(glm::vec3(0, 2.5, 0));
	renderComponent.SetMatrix(1, body.mat());
}
void MagicStaff::Update(float delta) {
	
}
std::vector<engine::ColliderComponent*> MagicStaff::GetColliders() {
	std::vector<engine::ColliderComponent*> out;
	return out;
}