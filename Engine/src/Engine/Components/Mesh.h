#pragma once

#include <string>
#include <vector>

#include "../Rendering/Shader.h"
#include "../Rendering/BufferContainer.h"
#include "Material.h"

namespace engine {

	class Mesh {
	public:
		Mesh();
		~Mesh();

		std::vector<Material*> materials;
		unsigned char shaderType = 0;
		BufferContainer container;
		void Draw() {
			container.Draw();
		}

		bool hasError = false;
	private:

	};
}