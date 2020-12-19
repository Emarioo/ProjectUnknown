#include "Goblin.h"

Goblin::Goblin(float x, float y, float z) {
	position = glm::vec3(x, y, z);

	slash.SetData(dManager::GetAnim("goblin_slash"));
	slash.running = true;

	bone.SetBone(dManager::GetBone("goblin_skeleton"));
	bone.SetAnim(&slash);

	mesh.AddMesh(dManager::GetMesh("goblin_body"));
	mesh.SetBone(&bone);

	
	/* Lock weapon?
	mesh.SetMatrix(weapon_index,body + getBoneMatrix)
	*/
}
void Goblin::PreComponents() {
	Location body;
	body.Translate(position);
	body.Rotate(rotation);

	mesh.SetMatrix(0, body.mat());
}
void Goblin::Update(float delta) {
	slash.Update(delta);
	
}