#pragma once

#include "glm/glm.hpp"
#include <vector>

#include "Data/BoneData.h"

#include "AnimationComponent.h"

class BoneComponent {
public:
	BoneComponent() {}

	BoneData* bone=nullptr;
	void SetBone(BoneData* data);

	AnimationComponent* anim=nullptr;
	void SetAnim(AnimationComponent* anim);

	void addToArray(glm::mat4* mat, Bone bone, glm::mat4 parent);
	/*
	Parameter is an array of glm::mat4 wih bone.count as size
	*/
	void GetBoneTransforms(glm::mat4* mat);

	bool hasError = false;
};