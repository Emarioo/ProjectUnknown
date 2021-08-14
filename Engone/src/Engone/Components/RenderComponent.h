#pragma once

#include "Animator.h"
#include "Model.h"
#include "../Handlers/AssetHandler.h"

namespace engine {

	class RenderComponent {
	public:
		RenderComponent() = default;

		Animator animator;
		Model* model=nullptr;
		/*
		Matrix which contains the position and rotation the renderer should use when rendering.
		It gets updated before calling the RenderObjects function
		*/
		glm::mat4 matrix=glm::mat4(1);
		
		void SetModel(const std::string& name);
		/*
		Parameter is an array of glm::mat4 with bones.size()
		*/
		void GetArmatureTransforms(std::vector<glm::mat4>& mats);

		bool hasError = false;

	private:

	};
}