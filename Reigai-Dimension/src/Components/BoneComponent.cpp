#include "BoneComponent.h"

void BoneComponent::SetBone(BoneData* data) {
	bone = data;
}
void BoneComponent::SetAnim(AnimationComponent* comp) {
	anim = comp;
}
void BoneComponent::addToArray(glm::mat4* mat, Bone bone, glm::mat4 parent) {
	if (anim != nullptr) {
		mat[bone.index] = parent * anim->GetTransform(bone.name);
	} else {
		mat[bone.index] = parent * bone.localMat;
	}
	for (Bone b : bone.children) {
		addToArray(mat, b, mat[bone.index]);
	}
}
void BoneComponent::GetBoneTransforms(glm::mat4* mat) {
	mat[bone->bone.index] = bone->bone.localMat;
	for (Bone b : bone->bone.children) {
		addToArray(mat, b, bone->bone.localMat);
	}
}