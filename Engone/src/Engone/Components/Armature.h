#pragma once

#include "../DebugTool/DebugHandler.h"

namespace engine {
	class Bone {
	public:
		unsigned short parent = 0;

		glm::mat4 localMat = glm::mat4(1);
		glm::mat4 invModel = glm::mat4(1);

		Bone() {}

	};
	class Armature {
	public:
		std::vector<Bone> bones;

		Armature() {}

		bool hasError = false;
	};
}