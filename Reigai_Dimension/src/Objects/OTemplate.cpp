#include "OTemplate.h"

OTemplate::OTemplate(float x, float y, float z) {
	position = glm::vec3(x, y, z);
	ANIM_NAME = new Animation("ANIM_ID");
	ANIM_NAME->running = true; // Start animation by default?
}
void OTemplate::Draw() {
	Location body;
	body.Translate(position);
	body.Rotate(rotation);
	DrawMesh("OBJECT_MESH", body.mat());

	body.Rotate(ANIM_NAME->GetRot("SUB_MESH"));
	body.Translate(ANIM_NAME->GetPos("SUB_MESH"));
	DrawMesh("OBJECT_SUB_MESH", body.mat());
}
void OTemplate::Update() {
	// Run animation
	if (ANIM_NAME != nullptr) ANIM_NAME->Update();
	// Meta Data
	MetaUpdate();
}