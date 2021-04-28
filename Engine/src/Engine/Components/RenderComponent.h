#pragma once

#include <glm/glm.hpp>
#include "Animator.h"
#include "Model.h"
#include "Managers/AssetManager.h"

namespace engine {

	class RenderComponent {
	public:
		RenderComponent();

		Animator animator;
		Model* model=nullptr;
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