#pragma once

#include <glm/glm.hpp>
#include "Data/MeshData.h"
#include "Components/BoneComponent.h"
#include "Components/AnimationComponent.h"

#include "Managers/DataManager.h"

namespace engine {

	class RenderComponent {
	public:
		RenderComponent();

		BoneComponent bone;
		AnimationComponent anim;
		void SetBone(const std::string& data);
		void SetAnim(const std::string& data);

		std::vector<MeshData*> meshes;
		std::vector<glm::mat4> matrices;
		void AddMesh(const std::string& data);
		void SetMatrix(int index, glm::mat4 m);

		bool hasError = false;
		bool enabled = false;

	private:

	};
}