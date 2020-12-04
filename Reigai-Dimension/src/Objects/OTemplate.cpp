#include "OTemplate.h"

OTemplate::OTemplate(float x, float y, float z) {
	SetPosition(x, y, z);
	SetName("OTemplate");

	// Load meshes
	mesh.AddMesh(dManager::GetMesh("vertexcolor"));
	dManager::AddComponent(&mesh);

	// Load animations
//	anim.SetAnim(dManager::GetAnim("animname"));
//	anim.running = true;

	// Load colliders
	//coll.SetData(dManager::GetColl("vertexcolor"));
	//dManager::AddComponent(&coll);// <- might not be neccesary
}

void OTemplate::PreComponents() {
	Location body;
	body.Translate(GetPos());
	body.Rotate(GetRot());
	//mesh.SetMatrix(body.mat());

	coll.SetMatrix(body.mat());

//	body.Rotate(idle.GetRot("fireball"));
//	body.Translate(idle.GetPos("fireball"));
//	fireball.SetMatrix(body.mat());
}
void OTemplate::Update(float delta) {
//	anim.Update(delta);
}
std::vector<ColliderComponent*> OTemplate::GetColliders() {
	std::vector<ColliderComponent*> out;
	//out.push_back(&coll);
	return out;
}