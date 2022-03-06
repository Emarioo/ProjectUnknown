#pragma once

#include "../Handlers/AssetHandler.h"
#include "Entity.h"

namespace engone {
	struct Collider {
		ColliderAsset* asset;
		Entity entity;

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};
}