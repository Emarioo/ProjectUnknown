#pragma once

#include "Animator.h"
#include "../Handlers/AssetManager.h"

namespace engone {

	class RenderComponent {
	public:
		RenderComponent() = default;

	//	Animator animator;
	//	Model* model=nullptr;
		/*
		Matrix which contains the position and rotation the renderer should use when rendering.
		It gets updated before calling the RenderObjects function.
		Changes to this function will not affect anything.
		*/
		glm::mat4 matrix=glm::mat4(1);
		ModelAsset* model;
		
		void GetInstanceTransforms(std::vector<glm::mat4>& mats);

		/*
		Argument is an array of glm::mat4 with bones.size()
		*/
		void GetArmatureTransforms(std::vector<glm::mat4>& mats);

		bool hasError = false;

	private:

	};
}