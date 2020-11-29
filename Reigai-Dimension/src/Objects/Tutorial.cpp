#include "Tutorial.h"

Tutorial::Tutorial(float x, float y, float z) {
	SetPosition(x, y, z);
	SetName("Tutorial");

	// Load meshes
	parkour.SetMesh(dManager::GetMesh("parkour"));
	dManager::AddComponent(&parkour);

	// Load animations

	// Load colliders
	collider.SetColl(dManager::GetColl("parkour"));
	dManager::AddComponent(&collider);

}
void Tutorial::PreComponents() {
	Location body;
	body.Translate(GetPos());
	body.Rotate(GetRot());

	parkour.SetMatrix(body.mat());
}
void Tutorial::Update(float delta) {
	
}
std::vector<ColliderComponent*> Tutorial::GetColliders() {
	std::vector<ColliderComponent*> out;
	out.push_back(&collider);
	return out;
}