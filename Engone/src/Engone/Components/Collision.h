#pragma once

#include "../Handlers/AssetHandler.h"
#include "Entity.h"

namespace engone {
	struct Collider {
		ColliderAsset* asset;
		Entity entity;

		glm::mat4 matrix;
		glm::vec3 position;
		glm::vec3 scale;
	};
}