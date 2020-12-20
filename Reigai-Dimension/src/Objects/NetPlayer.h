#pragma once

#include "GameObject.h"

class NetPlayer : public GameObject {
public:
	NetPlayer(AnimData *data, float x, float y, float z);
	void PreComponents() override;
	void Update(float delta) override;

	RenderComponent mesh;
	BoneComponent bone;
	AnimationComponent slash;

	//MeshData *player_body, *player_head, *player_ruarm, *player_rlarm;
	

};