#include "MagicStaff.h"

MagicStaff::MagicStaff(float x,float y,float z) {
	SetPosition(x, y, z);
	idle = new Animation("magicstaff_idle");
	idle->running = true;
}
void MagicStaff::Draw() {
	Location body;
	body.Translate(GetPos());
	body.Rotate(GetRot());
	
	DrawMesh("magicstaff_staff", body.mat());
	body.Rotate(idle->GetRot("fireball"));
	body.Translate(idle->GetPos("fireball"));
	DrawMesh("magicstaff_fireball", body.mat());
}
void MagicStaff::Update(float delta) {
	if(idle!=nullptr)
		idle->Update(delta);
}