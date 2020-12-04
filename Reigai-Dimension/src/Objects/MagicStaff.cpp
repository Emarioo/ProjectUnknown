#include "MagicStaff.h"

MagicStaff::MagicStaff(float x,float y,float z) {
	SetPosition(x, y, z);
	SetName("MagicStaff");

	// Load meshes
	staff.AddMesh(dManager::GetMesh("staff"));
	dManager::AddComponent(&staff);
	fireball.AddMesh(dManager::GetMesh("fireball"));
	dManager::AddComponent(&fireball);

	// Load animations
	//idle.SetData(dManager::GetAnim("idle"));
	//idle.running = true;

	// Load colliders

}

void MagicStaff::PreComponents() {
	/*Location body;
	body.Translate(GetPos());
	body.Rotate(GetRot());
	staff.SetMatrix(body.mat());
	
	//body.Rotate(idle.GetRot("fireball"));
	//body.Translate(idle.GetPos("fireball"));
	fireball.SetMatrix(body.mat());
	*/

}
void MagicStaff::Update(float delta) {
	//idle.Update(delta);
}
std::vector<ColliderComponent*> MagicStaff::GetColliders() {
	std::vector<ColliderComponent*> out;
	return out;
}