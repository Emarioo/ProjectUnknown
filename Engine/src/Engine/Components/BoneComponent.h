#pragma once

#include "glm/glm.hpp"
#include <vector>

#include "../Data/BoneData.h"

#include "AnimationComponent.h"
namespace engine {
	class BoneComponent {
	public:
		BoneComponent() {}

		BoneData* bone = nullptr;
		void SetBone(BoneData* data);

		AnimationComponent* anim = nullptr;
		void SetAnim(AnimationComponent* anim);

		/*
		Parameter is an array of glm::mat4 with bones.size()
		*/
		void GetBoneTransforms(std::vector<glm::mat4>& mats);

		bool enabled = false;
		bool hasError = false;
	};
}