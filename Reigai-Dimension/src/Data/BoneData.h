#pragma once

#include "glm/glm.hpp"
#include <vector>

class Bone {
public:
	std::vector<Bone> children;
	int index = 0;
	glm::mat4 localMat=glm::mat4(1);

	glm::mat4 inverseMat=glm::mat4(1);

	Bone() {}

	void AddChild(Bone child) {
		children.push_back(child);
	}
	void calcInverseBindTransform(glm::mat4 parentBindTransform) {
		glm::mat4 bindT = parentBindTransform * localMat;
		inverseMat = glm::inverse(bindT);
		for (Bone child : children) {
			child.calcInverseBindTransform(bindT);
		}
	}
};
class BoneData {
public:
	Bone bone;
	int count=0;

	BoneData(){}

	/*
	Called in the end of fManager::LoadBone(...)
	*/
	void calcInverseBindTransform() {
		bone.calcInverseBindTransform(glm::mat4(1));
	}

	bool hasError = false;
};