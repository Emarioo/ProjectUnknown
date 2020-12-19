#pragma once

#include "Objects/GameObject.h"
#include "Objects/ClientPlayer.h"

namespace oManager {

	void AddDimension(const std::string& s, Dimension dim);
	Dimension* GetDimension();
	void SetDimension(const std::string&);

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