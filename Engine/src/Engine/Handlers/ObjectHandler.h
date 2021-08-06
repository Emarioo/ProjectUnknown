#pragma once

#include "../World/Dimension.h"
#include "../Objects/GameObject.h"

#include "../Objects/Camera.h"

namespace engine {

	// Move this to ProjectUnknown?
	void AddDimension(const std::string& s, Dimension dim);
	Dimension* GetDimension();
	void SetDimension(const std::string&);

	Camera* GetCamera();

	GameObject* AddObject(GameObject* object);
	void DelObject(GameObject* object);
	void DelObject(const std::string& name);

	std::vector<GameObject*>& GetObjects();
	GameObject* GetObjectByName(const std::string& name);
}