#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Rendering/Shader.h"
#include "Rendering/BufferContainer.h"
#include "Rendering/Material.h"

namespace engine {

	class MeshData {
	public:
		MeshData() {}

		Material material;
		ShaderType shaderType = ShaderCurrent;
		BufferContainer container;
		void Draw() {
			container.Draw();
		}

		bool hasError = false;
	private:

	};
}