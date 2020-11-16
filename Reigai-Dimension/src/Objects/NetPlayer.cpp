#include "NetPlayer.h"

NetPlayer::NetPlayer(float x, float y, float z) {
	position = glm::vec3(x, y, z);
	idle = new Animation("player_idle");
	idle->running = true;
	idle->fps = 12;
}
void NetPlayer::Draw() {
	Location body;
	body.Translate(position);
	body.Rotate(rotation);

	DrawMesh("player_body", body.mat());
	
	Location head(body);
	head.Translate(idle->GetPos("Head"));
	DrawMesh("player_head", head.mat());

	Location arm(body);
	//arm.Translate(glm::vec3(render.GetMeshPos("player_ruarm").x,0,0));
	arm.Translate(idle->GetPos("RUArm"));
	arm.Rotate(idle->GetRot("RUArm"));
	DrawMesh("player_ruarm", arm.mat());

	arm.Translate(glm::vec3(0, -0.814433,0));
	DrawMesh("player_rlarm", arm.mat());
}
void NetPlayer::Update(float delta) {
	if (idle != nullptr)
		idle->Update(delta);
}