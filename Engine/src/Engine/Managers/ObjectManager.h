#pragma once

#include "Objects/ClientPlayer.h"
#include "World/Dimension.h"

namespace engine {

	void AddDimension(const std::string& s, Dimension dim);
	Dimension* GetDimension();
	void SetDimension(const std::string&);

	GameObject* AddObject(GameObject* object);
	void DelObject(GameObject* object);
	void DelObject(const std::string& name);

	ClientPlayer* GetPlayer();

	std::vector<GameObject*>& GetObjects();
	GameObject* GetObjectByName(const std::string& name);
	
	void AddPlayer(ClientPlayer* player);
}