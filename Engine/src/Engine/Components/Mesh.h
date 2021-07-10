#pragma once

#include <string>
#include <vector>

#include "../Rendering/Shader.h"
#include "../Rendering/BufferContainer.h"
#include "Material.h"

namespace engine {

	class Mesh {
	public:
		Mesh() {}

		std::vector<Material*> materials;
		ShaderType shaderType = ShaderCurrent;
		BufferContainer container;
		void Draw() {
			container.Draw();
		}

		bool hasError = false;
	private:

	};
}