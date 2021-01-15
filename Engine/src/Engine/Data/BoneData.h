#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <string>

#include "Utility/Debugger.h"
namespace engine {
	class Bone {
	public:
		int parent = 0;

		glm::mat4 localMat = glm::mat4(1);
		glm::mat4 invModel = glm::mat4(1);

		Bone() {}

	};
	class BoneData {
	public:
		std::vector<Bone> bones;

		BoneData() {}

		bool hasError = false;
	};
}