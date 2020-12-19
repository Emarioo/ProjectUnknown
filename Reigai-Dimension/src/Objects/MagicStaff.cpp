#include "MagicStaff.h"

MagicStaff::MagicStaff(float x,float y,float z) {
	SetPosition(x, y, z);
	SetName("MagicStaff");

	// Load meshes
	mesh.AddMesh(dManager::GetMesh("staff"));
	mesh.AddMesh(dManager::GetMesh("fireball"));
	dManager::AddComponent(&mesh);

	idle.SetData(dManager::GetAnim("MagicStaffIdle"));
	idle.running = true;
	// Load animations
	//idle.SetData(dManager::GetAnim("idle"));
	//idle.running = true;

	// Load colliders

}

void MagicStaff::PreComponents() {
	Location body;
	body.Translate(GetPos());
	body.Rotate(GetRot());
	mesh.SetMatrix(0,body.mat());
	
	body.Matrix(idle.GetTransform("fireball"));
	mesh.SetMatrix(1,body.mat());
}
void MagicStaff::Update(float delta) {
	//idle.Update(delta);
}
std::vector<ColliderComponent*> MagicStaff::GetColliders() {
	std::vector<ColliderComponent*> out;
	return out;
}