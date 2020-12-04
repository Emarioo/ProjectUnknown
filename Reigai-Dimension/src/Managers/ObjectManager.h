#pragma once

#include "Objects/GameObject.h"
#include "Objects/ClientPlayer.h"

namespace oManager {
	GameObject* AddObject(GameObject* object);
	void DelObject(GameObject* object);
	void DelObject(const std::string& name);

	ClientPlayer* GetPlayer();

	std::vector<GameObject*>* GetObjects();
	/*
	Setup player
	*/
	void Init();
}