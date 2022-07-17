#include "NetPlayer.h"

namespace engine {

	NetPlayer::NetPlayer(float x, float y, float z) {
		SetPosition(x, y, z);
		/*
		mesh.AddMesh(dManager::GetMesh(""));


		slash.SetData(dManager::GetAnim("PlayerIdle"));
		slash.running = true;
		*/
	}
	void NetPlayer::Update(float delta) {
		/*if (idle != nullptr)
			idle->Update(delta);
		*/
	}
}