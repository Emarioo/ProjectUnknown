#pragma once

#include "Animator.h"
#include "../Handlers/AssetHandler.h"

namespace engone {

	class RenderComponent {
	public:
		RenderComponent() = default;

		Animator animator;
		/*
		Matrix which contains the position and rotation the renderer should use when rendering.
		It gets updated before calling the RenderObjects function.
		Changes to this function will not affect anything.
		*/
		glm::mat4 matrix=glm::mat4(1);
		ModelAsset* model;
		void SetModel(ModelAsset* modelAsset);
		
		// Will give a list of combined parent matrices to instances, do mats[i] * instance.localMat to get whole transform
		void GetParentTransforms(std::vector<glm::mat4>& mats);

		/*
		@instance: The armature instance. Not the mesh instance
		*/
		void GetArmatureTransforms(std::vector<glm::mat4>& mats, glm::mat4& instanceMat, AssetInstance& instance, ArmatureAsset* asset);

		bool hasError = false;

	private:

	};
}