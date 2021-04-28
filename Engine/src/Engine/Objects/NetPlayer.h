#pragma once

#include "GameObject.h"

namespace engine {
	class NetPlayer : public engine::GameObject {
	public:
		NetPlayer(float x, float y, float z);
		
		void Update(float delta) override;

		//RenderComponent mesh;
		//BoneComponent bone;
		//AnimationComponent slash;

		//MeshData *player_body, *player_head, *player_ruarm, *player_rlarm;


	};
}