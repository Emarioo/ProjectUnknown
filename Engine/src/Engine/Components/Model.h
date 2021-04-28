#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Mesh.h"
#include "Animation.h"
#include "Armature.h"

namespace engine {

	class Model {
	public:

		Model(){}

		std::vector<Mesh*> meshes;
		std::vector<glm::mat4> matrices;
		void AddMesh(const std::string& data,glm::mat4 matrix);

		std::vector<Animation*> animations;
		void AddAnimation(const std::string& name);
		Armature* armature=nullptr;
		void SetArmature(const std::string& name);
		std::string colliderName="";
		void SetCollider(const std::string& name);

		bool hasError=false;
		bool hasCollider=false;
	};
}