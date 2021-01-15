#include "BoneComponent.h"
namespace engine {
	void BoneComponent::SetBone(BoneData* data) {
		if (data != nullptr) {
			bone = data;
			enabled = true;
		}
	}
	void BoneComponent::SetAnim(AnimationComponent* comp) {
		anim = comp;
	}
	void BoneComponent::GetBoneTransforms(std::vector<glm::mat4>& mats) {

		std::vector<glm::mat4> modelT(bone->bones.size());
		glm::mat4 magic = { 1.0, 0.0, 0.0, 0.0,
			0.0, 0.0, -1.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 1.0
		};
		for (int i = 0; i < bone->bones.size(); i++) {
			glm::mat4 loc = bone->bones[i].localMat;
			glm::mat4 inv = bone->bones[i].invModel;
			glm::mat4 ani = anim->GetTransform(i);
			if (i == 0)
				modelT[0] = (loc)*ani;
			else
				modelT[i] = modelT[bone->bones[i].parent] * (loc * ani);

			mats[i] = (modelT[i]) * inv;
		}

	}
}