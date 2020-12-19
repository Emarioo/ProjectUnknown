#include "NetPlayer.h"

NetPlayer::NetPlayer(AnimData *data, float x, float y, float z) {
	position = glm::vec3(x, y, z);

	mesh.AddMesh(dManager::GetMesh(""));


	slash.SetData(dManager::GetAnim("PlayerIdle"));
	slash.running = true;
}
void NetPlayer::PreComponents() {
	/*
	Location body;
	body.Translate(position);
	body.Rotate(rotation);

	renderer::DrawMesh(player_body, body.mat());
	
	Location head(body);
	head.Translate(idle->GetPos("Head"));
	renderer::DrawMesh(player_head, head.mat());

	Location arm(body);
	//arm.Translate(glm::vec3(render.GetMeshPos("player_ruarm").x,0,0));
	arm.Translate(idle->GetPos("RUArm"));
	arm.Rotate(idle->GetRot("RUArm"));
	renderer::DrawMesh(player_ruarm, arm.mat());

	arm.Translate(glm::vec3(0, -0.814433,0));
	renderer::DrawMesh(player_rlarm, arm.mat());
	*/
}
void NetPlayer::Update(float delta) {
	/*if (idle != nullptr)
		idle->Update(delta);
	*/
}